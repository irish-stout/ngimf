#define CONTENT_SIZE 512

// Exit with print error mesage. 
void ngf_panic(const char* message);

// Make HTML content from file.
char* ngf_gen_content(char* file, char* content);

// GET response conetnt-type.
// File type
// HTML file:    text/html; charset=UTF-8
// CSS file:     text/css
// JS file:      application/javascript
// PNG file:     image/png
// JPEG file:    image/jpeg
// svg file:     image/svg+xml
// favicon file: image/vnd.microsoft.icon
char* ngf_resp_conent_type(char* buf);

void p(char* msg);
