# μki Library

A micro-wiki for personal stuff. Designed to be extremely simple and
straight-forward. This is the library that should be used to parse and render
everything related to a μki wiki. It's designed to be extremely embeddable.


## Library Usage

Using the library couldn't be any simpler. You can check out a more complete
example by looking at the [`src/main.c`](/src/main.c) file. Anyway, this is
a simple way to render a page given a `wiki_path` (the root of the μki) and
a `page_name`:

```c
#include <stdio.h>
#include <stdlib.h>
#include "uki.h"

int uki_error;
const char *wiki_path = "testwiki/";
const char *page_name = "HelloWorld";

int main(const int argc, const char **argv) {
	// Initialize the uki wiki.
	if ((uki_error = uki_initialize(wiki_path)) != UKI_OK) {
		fprintf(stderr, uki_error_msg(uki_error));
		uki_clean();

		return 1;
	}

	// Render a page.
	char *content;
	if ((uki_error = uki_render_page(&content, page_name)) != UKI_OK) {
		fprintf(stderr, "ERROR: %s", uki_error_msg(uki_error));
		uki_clean();

		return 1;
	}

	// Print the page content.
	printf("%s\n", content);

	// Clean up and return.
	uki_clean();
	return 0;
}
```

Let's assume that you have a project folder with the following structure.

```
.
├── libuki
│   └── (this repository)
├── testwiki
│   ├── pages
│   │   └── HelloWorld.html
│   ├── templates
│   │   └── container.html
│   ├── MANIFEST.uki
│   └── VARIABLES.uki
└── test.c
```

To compile and run this project just follow these simple steps for **UNIX**
systems:

```console
foo@bar:~/testuki$ cd libuki
foo@bar:~/testuki/libuki$ make
foo@bar:~/testuki/libuki$ cd ..
foo@bar:~/testuki$ gcc -Llibuki/build/lib -o testuki test.c -luki
foo@bar:~/testuki$ LD_LIBRARY_PATH=/home/username/foo:$LD_LIBRARY_PATH ./testuki
```

If you're running **Windows** it's just a matter of opening the Visual Studio
Workspace `*.dsw` file and hitting `Build`. This will build you a DLL that
you can use in your programs.


## License

This project is licensed under the **MIT License**.
