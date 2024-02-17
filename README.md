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
    - `var IDENTIFIER ASSIGN expr`
    - `comp-expr ((and|or) comp-expr)*`
- `comp-expr :`
    - `not comp-expr`
    - `arith-expr ((EQUAL|NEQ|LESS|GREATER|LEQ|GEQ) arith-expr)*`
- `arith-expr :`
    - `term ((ADD|SUB) term)*`
- `term :`
    - `factor ((MUL|DIV|MOD) factor)*`
- `factor :`
    - `(ADD|SUB) factor`
- `power :`
    - `atom (POW factor)*`
- `atom :`
    - `INT|FLOAT|IDENTIFIER`
    - `LEFT_PAREN expr RIGHT_PAREN`
- `if-expr :`
    - `if expr then expr (else (if-expr|expr))?`

### if statement

- `if condition then expression`
- `if condition then expression else expression`

```pseudo
if a < b and a >= 10 then
    var a <- 10
else if a < 20 then
    var a <- 1.5
else
    var a <- 8
```

### Function

Function begin with `Algorithm`, an example for adding two number would be

```pseudo
Algorithm add(a, b):
    return a + b
```