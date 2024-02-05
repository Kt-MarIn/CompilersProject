CC=gcc
FLEX=flex
BISON=bison


.PHONY: clean
.lex: lex.l
	$(FLEX) lex.l
.syntax: syntax.y
	$(BISON) -t -d -v syntax.y
bplc: .lex .syntax
	$(CC) syntax.tab.c tree.c -lfl -ly -o bin/bplc
clean:
	@rm -f lex.yy.c syntax.tab.* *.output *.so
