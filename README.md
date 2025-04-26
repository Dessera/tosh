# tosh

`tosh` is a toy shell which used to training my cplusplus programming skill.

## Features

- IO Redirection
- Home Directory replacement
- `sete/unsete` environment variables

Unsupported features:

- Pipe
- Embedded environment variables
- History
- Configuration file

> I do not plan to implement these features, because I have no time to focus on this project.

## Bugs

- `EventReader` is not a reliable event reader, if user's input is too fast, it will miss some events, which may cause unexpected behavior. I cannot fix this because I have no idea how to fix it.

- `Document` class is slow (especially when completing), because I do not implement operations for multiple characters (string) and steps.
