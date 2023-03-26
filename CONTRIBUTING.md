# Guidelines for contributing to Jurl
Thanks for taking the time to contribute!

Please read this document before making contributions. If you find anything missing of not appropriate, please do not hesitate and create an issue in this repository.

## Build Errors
Some build errors are due to bad code, while others are due to your system setup.
I can only help with the former.
To submit a build-error related issue, please include a reference to the exact part of the code causing the problem (legitimate instances of this will usually be related to undefined symbols due to a lack of versioning).

## Missing Features
If a feature of libcurl you depend on is missing, let me know!
It may be possible to add it.
Bonus points if you have an idea on how to represent it.

Please note that the high level `jurl` API is meant to be HTTP-only.
However, `jurl/native` is fully unopinionated, and can be used to build your own higher level API.
