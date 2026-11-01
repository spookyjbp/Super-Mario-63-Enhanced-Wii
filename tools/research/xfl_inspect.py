#!/usr/bin/env python3
"""Print an XFL symbol summary: SM63_SOURCE=<checkout> xfl_inspect.py "<symbol name>"."""
import os, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
import xfl
xfl.LIB = Path(os.environ.get('SM63_SOURCE', '.')) / 'Super_Mario_63_2010/LIBRARY'
for name in sys.argv[1:]:
    xfl.summary(name)
