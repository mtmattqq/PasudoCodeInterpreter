# PseudoCodeInterpreter

Let's make the pseudo code a actual programming language.

## Language

### Operators and Operations

- `a <- 10` : move the value 10 to the variable `a`
- `+` : addition 
- `-` : subtraction
- `*` : multiplication
- `/` : division
- `%` : mod operation
- `^` : power operation

### Expression

- `expr : term ((ADD|SUB) term)*`
- `term : factor ((MUL|DIV|MOD) factor)*`
- `factor :`
    - `INT|FLOAT`
    - `(ADD|SUB) factor`
    - `LEFT_PAREN expr RIGHT_PAREN`

### Function

Function begin with `Algorithm`, an example for adding two number would be

```pseudo
Algorithm add(a, b):
    return a + b
```