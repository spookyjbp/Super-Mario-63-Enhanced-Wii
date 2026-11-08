#!/usr/bin/env python3
"""A small ActionScript 2 interpreter for the scripts FFDec decompiles from
the original's cutscenes (tools/cinema_sim.py): statements, expressions,
functions and closures, if / while / for, tellTarget, object literals. The
host objects (movie clips, Sound, Color, _root) come from the caller.

Only what those scripts use is supported; anything else raises AS2Error with
where it is, so a new script cannot be misread silently."""
import math
import re


class AS2Error(Exception):
    pass


# ---- tokens ----------------------------------------------------------------
TOKEN = re.compile(r'''
    (?P<ws>\s+|//[^\n]*|/\*.*?\*/)
  | (?P<num>\d+\.\d*(?:[eE][-+]?\d+)?|\.\d+(?:[eE][-+]?\d+)?|\d+(?:[eE][-+]?\d+)?|0x[0-9a-fA-F]+)
  | (?P<str>"(?:[^"\\]|\\.)*"|'(?:[^'\\]|\\.)*')
  | (?P<id>[A-Za-z_$][A-Za-z0-9_$]*)
  | (?P<op>>>>|===|!==|<<|>>|==|!=|<=|>=|&&|\|\||\+\+|--|\+=|-=|\*=|/=|%=|[-+*/%<>=!?:.,;(){}\[\]&|^~])
''', re.S | re.X)
KEYWORDS = {'if', 'else', 'while', 'for', 'function', 'var', 'return', 'new', 'delete', 'true', 'false', 'null',
            'undefined', 'tellTarget', 'typeof', 'break', 'continue', 'this'}


def _unescape(s):
    out, i = [], 0
    while i < len(s):
        c = s[i]
        if c == '\\' and i + 1 < len(s):
            n = s[i + 1]
            out.append({'n': '\n', 'r': '\r', 't': '\t'}.get(n, n)); i += 2
        else:
            out.append(c); i += 1
    return ''.join(out)


def tokens(src):
    out, pos = [], 0
    while pos < len(src):
        m = TOKEN.match(src, pos)
        if not m:
            raise AS2Error(f'cannot read {src[pos:pos + 30]!r}')
        pos = m.end()
        kind = m.lastgroup
        if kind == 'ws':
            continue
        text = m.group()
        if kind == 'num':
            out.append(('num', float(int(text, 16)) if text.startswith('0x') else float(text)))
        elif kind == 'str':
            out.append(('str', _unescape(text[1:-1])))
        elif kind == 'id':
            out.append(('kw' if text in KEYWORDS else 'id', text))
        else:
            out.append(('op', text))
    out.append(('eof', None))
    return out


# ---- parser: statements as tuples ------------------------------------------
class Parser:
    BIN = {'||': 1, '&&': 2, '|': 3, '^': 4, '&': 5, '==': 6, '!=': 6, '===': 6, '!==': 6,
           '<': 7, '>': 7, '<=': 7, '>=': 7, '<<': 8, '>>': 8, '>>>': 8, '+': 9, '-': 9, '*': 10, '/': 10, '%': 10}

    def __init__(self, src):
        self.t = tokens(src)
        self.i = 0

    def peek(self, k=0):
        return self.t[self.i + k]

    def next(self):
        tok = self.t[self.i]; self.i += 1
        return tok

    def accept(self, kind, value=None):
        tok = self.peek()
        if tok[0] == kind and (value is None or tok[1] == value):
            self.i += 1
            return tok
        return None

    def expect(self, kind, value=None):
        tok = self.accept(kind, value)
        if not tok:
            raise AS2Error(f'expected {value or kind}, found {self.peek()}')
        return tok

    def program(self):
        body = []
        while self.peek()[0] != 'eof':
            body.append(self.statement())
        return ('block', body)

    def block(self):
        if self.accept('op', '{'):
            body = []
            while not self.accept('op', '}'):
                body.append(self.statement())
            return ('block', body)
        return self.statement()

    def statement(self):
        tok = self.peek()
        if tok == ('op', ';'):
            self.next(); return ('block', [])
        if tok == ('op', '{'):
            return self.block()
        if tok[0] == 'kw':
            k = tok[1]
            if k == 'if':
                self.next(); self.expect('op', '('); c = self.expr(); self.expect('op', ')')
                a = self.block()
                b = self.block() if self.accept('kw', 'else') else None
                return ('if', c, a, b)
            if k == 'while':
                self.next(); self.expect('op', '('); c = self.expr(); self.expect('op', ')')
                return ('while', c, self.block())
            if k == 'for':
                self.next(); self.expect('op', '(')
                init = None if self.peek() == ('op', ';') else (self.var_decl() if self.peek() == ('kw', 'var') else ('expr', self.expr()))
                self.expect('op', ';')
                cond = None if self.peek() == ('op', ';') else self.expr()
                self.expect('op', ';')
                step = None if self.peek() == ('op', ')') else self.expr()
                self.expect('op', ')')
                return ('for', init, cond, step, self.block())
            if k == 'function' and self.peek(1)[0] == 'id':
                self.next(); name = self.next()[1]
                return ('var', [(name, self.function_rest())])
            if k == 'var':
                d = self.var_decl(); self.accept('op', ';'); return d
            if k == 'return':
                self.next()
                v = None if self.peek() in (('op', ';'), ('op', '}')) else self.expr()
                self.accept('op', ';'); return ('return', v)
            if k in ('break', 'continue'):
                self.next(); self.accept('op', ';'); return (k,)
            if k == 'tellTarget':
                self.next(); self.expect('op', '('); t = self.expr(); self.expect('op', ')')
                return ('tell', t, self.block())
            if k == 'delete':
                self.next(); e = self.expr(); self.accept('op', ';'); return ('delete', e)
        e = self.expr()
        self.accept('op', ';')
        return ('expr', e)

    def var_decl(self):
        self.expect('kw', 'var')
        out = []
        while True:
            name = self.expect('id')[1]
            out.append((name, self.expr() if self.accept('op', '=') else ('lit', None)))
            if not self.accept('op', ','):
                return ('var', out)

    def function_rest(self):
        self.expect('op', '(')
        params = []
        while not self.accept('op', ')'):
            params.append(self.expect('id')[1]); self.accept('op', ',')
        self.expect('op', '{')
        body = []
        while not self.accept('op', '}'):
            body.append(self.statement())
        return ('func', params, ('block', body))

    def expr(self):
        return self.assign()

    def assign(self):
        left = self.ternary()
        tok = self.peek()
        if tok[0] == 'op' and tok[1] in ('=', '+=', '-=', '*=', '/=', '%='):
            self.next()
            right = self.assign()
            if left[0] not in ('name', 'member', 'index'):
                raise AS2Error(f'cannot assign to {left}')
            return ('assign', tok[1], left, right)
        return left

    def ternary(self):
        c = self.binary(1)
        if self.accept('op', '?'):
            a = self.assign(); self.expect('op', ':'); b = self.assign()
            return ('cond', c, a, b)
        return c

    def binary(self, level):
        left = self.unary()
        while True:
            tok = self.peek()
            if tok[0] != 'op' or tok[1] not in self.BIN or self.BIN[tok[1]] < level:
                return left
            op = tok[1]; self.next()
            right = self.binary(self.BIN[op] + 1)
            left = ('bin', op, left, right)

    def unary(self):
        tok = self.peek()
        if tok[0] == 'op' and tok[1] in ('-', '+', '!', '~'):
            self.next(); return ('un', tok[1], self.unary())
        if tok[0] == 'op' and tok[1] in ('++', '--'):
            self.next(); return ('pre', tok[1], self.unary())
        if tok == ('kw', 'typeof'):
            self.next(); return ('typeof', self.unary())
        if tok == ('kw', 'new'):
            self.next()
            ctor = self.primary()
            while self.accept('op', '.'):
                ctor = ('member', ctor, self.expect('id')[1])
            args = self.args() if self.peek() == ('op', '(') else []
            return self.postfix(('new', ctor, args))
        return self.postfix(self.primary())

    def args(self):
        self.expect('op', '(')
        out = []
        while not self.accept('op', ')'):
            out.append(self.expr()); self.accept('op', ',')
        return out

    def postfix(self, e):
        while True:
            if self.accept('op', '.'):
                tok = self.next()
                if tok[0] not in ('id', 'kw'):
                    raise AS2Error(f'member name expected, found {tok}')
                e = ('member', e, tok[1])
            elif self.peek() == ('op', '['):
                self.next(); k = self.expr(); self.expect('op', ']'); e = ('index', e, k)
            elif self.peek() == ('op', '('):
                e = ('call', e, self.args())
            elif self.peek()[0] == 'op' and self.peek()[1] in ('++', '--'):
                e = ('post', self.next()[1], e)
            else:
                return e

    def primary(self):
        tok = self.next()
        kind, v = tok
        if kind == 'num' or kind == 'str':
            return ('lit', v)
        if kind == 'kw':
            if v == 'true': return ('lit', True)
            if v == 'false': return ('lit', False)
            if v in ('null', 'undefined'): return ('lit', None)
            if v == 'this': return ('this',)
            if v == 'function': return self.function_rest()
        if kind == 'id':
            return ('name', v)
        if tok == ('op', '('):
            e = self.expr(); self.expect('op', ')'); return e
        if tok == ('op', '{'):
            items = []
            while not self.accept('op', '}'):
                k = self.next()[1]; self.expect('op', ':'); items.append((str(k) if not isinstance(k, str) else k, self.expr()))
                self.accept('op', ',')
            return ('obj', items)
        if tok == ('op', '['):
            items = []
            while not self.accept('op', ']'):
                items.append(self.expr()); self.accept('op', ',')
            return ('arr', items)
        raise AS2Error(f'unexpected {tok}')


def parse(src):
    return Parser(src).program()


# ---- values ----------------------------------------------------------------
class Undefined:
    def __repr__(self):
        return 'undefined'


UNDEF = Undefined()


class Obj:
    """A plain AS2 object (and the base of host objects)."""
    def __init__(self, props=None):
        self.props = dict(props or {})

    def get(self, name):
        return self.props.get(name, UNDEF)

    def set(self, name, value):
        self.props[name] = value

    def delete(self, name):
        self.props.pop(name, None)


class Function:
    def __init__(self, params, body, scope, owner):
        self.params, self.body, self.scope, self.owner = params, body, scope, owner


class Native:
    def __init__(self, fn):
        self.fn = fn


class Return(Exception):
    def __init__(self, value):
        self.value = value


class Break(Exception):
    pass


class Continue(Exception):
    pass


def num(v):
    if v is UNDEF or v is None:
        return float('nan')     # SWF 7+ (the game is SWF 8)
    if isinstance(v, bool):
        return 1.0 if v else 0.0
    if isinstance(v, (int, float)):
        return float(v)
    if isinstance(v, str):
        try:
            return float(v)
        except ValueError:
            return float('nan')
    return float('nan')


def string(v):
    if v is UNDEF:
        return 'undefined'      # SWF 7+
    if v is None:
        return 'null'
    if isinstance(v, bool):
        return 'true' if v else 'false'
    if isinstance(v, float):
        if v != v:
            return 'NaN'
        if v in (float('inf'), float('-inf')):
            return 'Infinity' if v > 0 else '-Infinity'
        return str(int(v)) if v == int(v) and abs(v) < 1e15 else repr(v)
    if isinstance(v, str):
        return v
    return '[object]'


def toint(v):
    x = num(v)
    if x != x or x in (float('inf'), float('-inf')):
        return 0
    return int(x)


def truth(v):
    if v is UNDEF or v is None:
        return False
    if isinstance(v, bool):
        return v
    if isinstance(v, (int, float)):
        return v != 0 and v == v
    if isinstance(v, str):
        return v != ''   # SWF 7+
    return True


def equal(a, b):
    if a is UNDEF or a is None:
        return b is UNDEF or b is None
    if b is UNDEF or b is None:
        return False
    if isinstance(a, str) and isinstance(b, str):
        return a == b
    if isinstance(a, (int, float, bool)) or isinstance(b, (int, float, bool)):
        if isinstance(a, (str,)) or isinstance(b, (str,)):
            return num(a) == num(b)
        return num(a) == num(b)
    return a is b


def strict_equal(a, b):
    if type(a) != type(b):
        if isinstance(a, (int, float)) and isinstance(b, (int, float)) and not isinstance(a, bool) and not isinstance(b, bool):
            return float(a) == float(b)
        return False
    if isinstance(a, (str, bool, float, int)):
        return a == b
    return a is b


# ---- the interpreter -------------------------------------------------------
class Scope:
    """A link of the scope chain: a function's locals (`vars`) or an object
    whose properties are variables (`obj`: the timeline of frame code, a
    tellTarget's target)."""
    def __init__(self, vars=None, obj=None, parent=None):
        self.vars, self.obj, self.parent = vars, obj, parent


class Interp:
    """Runs parsed AS2. `globals_` holds the global names (_root, Math,
    random, Sound...). Unqualified names go up the scope chain (locals, the
    timeline or target objects), then to the globals, as in AVM1 where a
    function's chain is the one it was defined in."""

    def __init__(self, globals_):
        self.globals = globals_
        self.steps = 0
        self.missing = {}       # calls of undefined functions: name -> count

    def run(self, program, this):
        try:
            self.exec(program, Scope(obj=this), this)
        except Return:
            pass

    def find(self, name, scope):
        """(value, holder): holder is the object the name was found on (or None)."""
        if name == '_root':
            return self.globals['_root'], None
        s = scope
        while s:
            if s.vars is not None:
                if name in s.vars:
                    return s.vars[name], None
            elif s.obj is not None:
                v = s.obj.get(name)
                if v is not UNDEF:
                    return v, s.obj
            s = s.parent
        return self.globals.get(name, UNDEF), None

    def lookup(self, name, scope, this):
        return self.find(name, scope)[0]

    def store(self, name, value, scope, this):
        s, first = scope, None
        while s:
            if s.vars is not None:
                if name in s.vars:
                    s.vars[name] = value
                    return
            elif s.obj is not None:
                if first is None:
                    first = s.obj
                if s.obj.get(name) is not UNDEF:
                    s.obj.set(name, value)
                    return
            s = s.parent
        (first if first is not None else this).set(name, value)

    def exec(self, st, scope, this):
        self.steps += 1
        if self.steps > 5_000_000:
            raise AS2Error('runaway script')
        k = st[0]
        if k == 'block':
            for s in st[1]:
                self.exec(s, scope, this)
        elif k == 'expr':
            self.eval(st[1], scope, this)
        elif k == 'var':
            for name, e in st[1]:
                v = self.eval(e, scope, this)
                if scope.vars is not None:
                    scope.vars[name] = v
                else:                           # timeline code: vars live on the timeline
                    scope.obj.set(name, v)
        elif k == 'if':
            if truth(self.eval(st[1], scope, this)):
                self.exec(st[2], scope, this)
            elif st[3] is not None:
                self.exec(st[3], scope, this)
        elif k == 'while':
            n = 0
            while truth(self.eval(st[1], scope, this)):
                try:
                    self.exec(st[2], scope, this)
                except Break:
                    break
                except Continue:
                    pass
                n += 1
                if n > 100000:
                    raise AS2Error('endless loop')
        elif k == 'for':
            if st[1] is not None:
                self.exec(st[1], scope, this)
            while st[2] is None or truth(self.eval(st[2], scope, this)):
                try:
                    self.exec(st[4], scope, this)
                except Break:
                    break
                except Continue:
                    pass
                if st[3] is not None:
                    self.eval(st[3], scope, this)
        elif k == 'return':
            raise Return(UNDEF if st[1] is None else self.eval(st[1], scope, this))
        elif k == 'break':
            raise Break()
        elif k == 'continue':
            raise Continue()
        elif k == 'tell':
            target = self.eval(st[1], scope, this)
            if isinstance(target, str):
                target = this.get(target)
            if not isinstance(target, Obj):
                return
            self.exec(st[2], Scope(obj=target, parent=scope), target)
        elif k == 'delete':
            e = st[1]
            if e[0] == 'member':
                o = self.eval(e[1], scope, this)
                if isinstance(o, Obj):
                    o.delete(e[2])
            elif e[0] == 'name':
                s_ = scope
                while s_ and s_.vars is None and s_.obj is None:
                    s_ = s_.parent
                if s_ is not None and s_.vars is not None:
                    s_.vars.pop(e[1], None)
                elif s_ is not None:
                    s_.obj.delete(e[1])
        else:
            raise AS2Error(f'statement {k}')

    def call(self, fn, this, args):
        if isinstance(fn, Native):
            return fn.fn(this, *args)
        if isinstance(fn, Function):
            vars_ = {p: (args[i] if i < len(args) else UNDEF) for i, p in enumerate(fn.params)}
            vars_['arguments'] = Obj({'length': float(len(args))})
            try:
                self.exec(fn.body, Scope(vars=vars_, parent=fn.scope), this if this is not None else fn.owner)
            except Return as r:
                return r.value
            return UNDEF
        if fn is None or fn is UNDEF:
            return UNDEF        # FFDec writes `onEnterFrame = null();`
        raise AS2Error(f'not a function: {fn!r}')

    def member_get(self, o, name):
        if isinstance(o, Obj):
            return o.get(name)
        if isinstance(o, str):
            if name == 'length':
                return float(len(o))
            if name == 'charAt':
                return Native(lambda this, i: o[toint(i)] if 0 <= toint(i) < len(o) else '')
            if name == 'toUpperCase':
                return Native(lambda this: o.upper())
            if name == 'toLowerCase':
                return Native(lambda this: o.lower())
            if name == 'substr':
                return Native(lambda this, a, b=UNDEF: o[toint(a):] if b is UNDEF else o[toint(a):toint(a) + toint(b)])
        if isinstance(o, list):
            if name == 'length':
                return float(len(o))
            if name == 'push':
                return Native(lambda this, *v: (o.extend(v), float(len(o)))[1])
        return UNDEF

    def member_set(self, o, name, value):
        if isinstance(o, Obj):
            o.set(name, value)
        elif isinstance(o, list) and name == 'length':
            del o[toint(value):]
        # anything else (a missing clip, a string, a number): Flash ignores it

    def ref(self, e, scope, this):
        """(get, set) for an assignable expression."""
        if e[0] == 'name':
            return (lambda: self.lookup(e[1], scope, this), lambda v: self.store(e[1], v, scope, this))
        if e[0] == 'member':
            o = self.eval(e[1], scope, this)
            return (lambda: self.member_get(o, e[2]), lambda v: self.member_set(o, e[2], v))
        if e[0] == 'index':
            o = self.eval(e[1], scope, this); key = self.eval(e[2], scope, this)
            if isinstance(o, list):
                def setl(v):
                    i = toint(key)
                    while len(o) <= i:
                        o.append(UNDEF)
                    o[i] = v
                return (lambda: o[toint(key)] if 0 <= toint(key) < len(o) else UNDEF, setl)
            k = string(key)
            return (lambda: self.member_get(o, k), lambda v: self.member_set(o, k, v))
        raise AS2Error(f'not assignable: {e}')

    def eval(self, e, scope, this):
        k = e[0]
        if k == 'lit':
            return e[1]
        if k == 'name':
            return self.lookup(e[1], scope, this)
        if k == 'this':
            return this
        if k == 'member':
            return self.member_get(self.eval(e[1], scope, this), e[2])
        if k == 'index':
            get, _ = self.ref(e, scope, this)
            return get()
        if k == 'call':
            f = e[1]
            if f[0] == 'member':
                o = self.eval(f[1], scope, this)
                fn = self.member_get(o, f[2])
                args = [self.eval(a, scope, this) for a in e[2]]
                if fn is UNDEF and o is not UNDEF:
                    self.missing[f[2]] = self.missing.get(f[2], 0) + 1
                return self.call(fn, o if isinstance(o, Obj) else this, args)
            if f[0] == 'lit' and f[1] is None:
                return None
            if f[0] == 'name':
                fn, holder = self.find(f[1], scope)
                args = [self.eval(a, scope, this) for a in e[2]]
                if fn is UNDEF:
                    self.missing[f[1]] = self.missing.get(f[1], 0) + 1
                return self.call(fn, holder if holder is not None else this, args)
            fn = self.eval(f, scope, this)
            args = [self.eval(a, scope, this) for a in e[2]]
            return self.call(fn, this, args)
        if k == 'assign':
            get, put = self.ref(e[2], scope, this)
            v = self.eval(e[3], scope, this)
            op = e[1]
            if op != '=':
                cur = get()
                if op == '+=':
                    v = string(cur) + string(v) if isinstance(cur, str) or isinstance(v, str) else num(cur) + num(v)
                elif op == '-=':
                    v = num(cur) - num(v)
                elif op == '*=':
                    v = num(cur) * num(v)
                elif op == '/=':
                    v = num(cur) / num(v) if num(v) else float('inf')
                elif op == '%=':
                    v = math.fmod(num(cur), num(v))
            put(v)
            return v
        if k == 'bin':
            op = e[1]
            if op == '&&':
                a = self.eval(e[2], scope, this)
                return self.eval(e[3], scope, this) if truth(a) else a
            if op == '||':
                a = self.eval(e[2], scope, this)
                return a if truth(a) else self.eval(e[3], scope, this)
            a = self.eval(e[2], scope, this); b = self.eval(e[3], scope, this)
            if op == '+':
                if isinstance(a, str) or isinstance(b, str):
                    return string(a) + string(b)
                return num(a) + num(b)
            if op == '-': return num(a) - num(b)
            if op == '*': return num(a) * num(b)
            if op == '/':
                d = num(b)
                return num(a) / d if d else (float('inf') if num(a) > 0 else float('-inf') if num(a) < 0 else float('nan'))
            if op == '%': return math.fmod(num(a), num(b)) if num(b) else float('nan')
            if op == '==': return equal(a, b)
            if op == '!=': return not equal(a, b)
            if op == '===': return strict_equal(a, b)
            if op == '!==': return not strict_equal(a, b)
            if op in ('<', '>', '<=', '>='):
                if isinstance(a, str) and isinstance(b, str):
                    x, y = a, b
                else:
                    x, y = num(a), num(b)
                return {'<': x < y, '>': x > y, '<=': x <= y, '>=': x >= y}[op]
            if op == '<<': return float(((toint(a) << (toint(b) & 31)) + 2**31) % 2**32 - 2**31)
            if op == '>>': return float(toint(a) >> (toint(b) & 31))
            if op == '>>>': return float((toint(a) % 2**32) >> (toint(b) & 31))
            if op == '&': return float(toint(a) & toint(b))
            if op == '|': return float(toint(a) | toint(b))
            if op == '^': return float(toint(a) ^ toint(b))
            raise AS2Error(f'operator {op}')
        if k == 'un':
            v = self.eval(e[2], scope, this)
            if e[1] == '-': return -num(v)
            if e[1] == '+': return num(v)
            if e[1] == '!': return not truth(v)
            if e[1] == '~': return float(~toint(v))
        if k in ('pre', 'post'):
            get, put = self.ref(e[2], scope, this)
            old = num(get())
            new = old + (1 if e[1] == '++' else -1)
            put(new)
            return new if k == 'pre' else old
        if k == 'cond':
            return self.eval(e[2] if truth(self.eval(e[1], scope, this)) else e[3], scope, this)
        if k == 'func':
            return Function(e[1], e[2], scope, this)
        if k == 'obj':
            return Obj({name: self.eval(v, scope, this) for name, v in e[1]})
        if k == 'arr':
            return [self.eval(v, scope, this) for v in e[1]]
        if k == 'new':
            ctor = self.eval(e[1], scope, this)
            args = [self.eval(a, scope, this) for a in e[2]]
            if isinstance(ctor, Native):
                return ctor.fn(None, *args)
            if ctor is UNDEF:
                return Obj()
            raise AS2Error(f'cannot construct {e[1]}')
        if k == 'typeof':
            v = self.eval(e[1], scope, this)
            return 'undefined' if v is UNDEF else 'number' if isinstance(v, float) else 'string' if isinstance(v, str) \
                else 'boolean' if isinstance(v, bool) else 'function' if isinstance(v, (Function, Native)) else 'object'
        raise AS2Error(f'expression {k}')
