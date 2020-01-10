gcc -o json *.c -DSJB -DDEBUG -I../../ -L../../lib/x86/ -lcjson -lm

./json parm.json

gcc -o xml *.c -DSXB -DDEBUG -I../../ -L../../lib/x86/ -lmxml

./xml parm.xml