## How to set up project in VS Code

If you use C/C++ official extension, create folder .vscode and settings.json in it, fill it with following:
```
{
    "C_Cpp.default.includePath": ["${workspaceRoot}"]
}
```
now it will not give error to include directives
