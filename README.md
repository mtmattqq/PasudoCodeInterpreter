# PseudoCodeInterpreter

For DSA textbook

## Language

### Operators and Operations

- `a <- 10` : move the value 10 to the variable `a`
- `+-*/%` : `%` means mod.

### Expression

- `expr` : `term ((ADD|SUB) term)*`
- `term` : `factor ((MUL|DIV|MOD) factor)*`
- `factor`: `INT|FLOAT`


### Function

Function begin with `Algorithm`, an example for adding two number would be

```pseudo
Algorithm add(a, b):
    return a + b
```