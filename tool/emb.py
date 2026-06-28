import sys
import os
import pathlib

ESCAPE = ["\\", "\"", "\'", "\\n", "\\r", "\\t"]
ESCAPE_T = [(w, "\\%s" % w) for w in ESCAPE]

infiles = [(open(p, "r", encoding="utf-8"), p.name)
           for p in pathlib.Path(sys.argv[1]).glob("*.cl")]
outname = sys.argv[2]
os.makedirs(os.path.dirname(outname), exist_ok=True)
outfile = open(outname, "w", encoding="utf-8")
outfile.write("#pragma once\n\nconst char* cl_source =")
for i in infiles:
    print(f"Emb: {i[1]}")
    outfile.write(f"\n#line 1 \"cl/{i[1]}\"\n")
    content = i[0].read()
    for w in ESCAPE_T:
        content = content.replace(w[0], w[1])
    lines = content.splitlines()
    outfile.write('\n'.join((f"\"{l}\"" for l in lines)))
outfile.write(";\n")
print("Emb Finnished")
