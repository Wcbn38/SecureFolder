# SecureFolder - A folder cypherer developped in C++

This project has been created to cypher entire folder with a password or other keys. It was designed to be highly robust.

Official project can be accessed on [github](https://github.com/Wcbn38/SecureFolder)

The project is divided in 3 sub-projects:
- SecureFolderCore
- SecureFolderCMD
- SecureFolderMFC

## SecureFolderCore

This sub-project contains the core of the cypherer. 2 function are essential:
`cypher` and `decypher`.
It also includes two usefull function `mapFolder` and `unmapFolder`

It is to note that the SHA256 key generator is standalone.

Only a few option are handled.

## SecureFolderCMD

This folder is for the command line project. It is actually only used for tests.

## SecureFolderMFC (GUI)

This folder contains the windows interface. It currently handles no custom options for the user.