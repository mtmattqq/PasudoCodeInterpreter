# PseudoCodeInterpreter

Let's make the pseudo code a actual programming language.

## Language

### Operators and Operations

- `var a <- 10` : move the value 10 to the variable `a`
- `+` : addition 
- `-` : subtraction
- `*` : multiplication
- `/` : division
- `%` : mod operation
- `^` : power operation

### Expression

- `expr :`
    - `KEYWORD:var IDENTIFIER ASSIGN expr`
    - `term ((ADD|SUB) term)*`
- `term : factor ((MUL|DIV|MOD) factor)*`
- `factor :`
    - `(ADD|SUB) factor`
- `power :`
    - atom (POW factor)*
- `atom :`
    - `INT|FLOAT|IDENTIFIER`
    - `LEFT_PAREN expr RIGHT_PAREN`


### Function

Function begin with `Algorithm`, an example for adding two number would be

```pseudo
Algorithm add(a, b):
    return a + b
```