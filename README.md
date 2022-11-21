# se-itmo-proglangs-astparser
C parser of infix notation expression to AST tree, its evaluation, RPN printer

## Operators
  - logical operators:
    - `&&`, `||`, `~`, `->`, `<->`
  - math operators:
    - `*`, `/`(div), `+`, `-`, `%`
  - unary math operators:
    - `-`, `!`(factorial)
  - brackets `(` and `)`

## Compile
```
make parser
```

## Example

Input:
```
(!5/(!2*!(5-2))+!5/(!3*!(5-3)))
```
Output:
```
-> OPEN(0) -> FACT(0) -> LIT(5) -> DIV(0) -> OPEN(0) -> FACT(0) -> LIT(2) -> MUL(0) -> FACT(0) -> OPEN(0) -> LIT(5) -> M
INUS(0) -> LIT(2) -> CLOSE(0) -> CLOSE(0) -> PLUS(0) -> FACT(0) -> LIT(5) -> DIV(0) -> OPEN(0) -> FACT(0) -> LIT(3) -> M
UL(0) -> FACT(0) -> OPEN(0) -> LIT(5) -> MINUS(0) -> LIT(3) -> CLOSE(0) -> CLOSE(0) -> CLOSE(0) ->
AST:
((!(5))/((!(2))*(!((5)-(2)))))+((!(5))/((!(3))*(!((5)-(3)))))
Infix notation:
(!5/(!2*!(5-2))+!5/(!3*!(5-3))) = 20
Reverse polish notation:
5 ! 2 ! 5 2 - ! * / 5 ! 3 ! 5 3 - ! * / +  = 20
```
