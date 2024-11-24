#define CONTENT_SIZE 512

typedef struct
{
  char *data;
  size_t size;
} ngfFile;

typedef struct
{
  char *protocol;
  size_t statusCode;
  char *reason;
  size_t contentLength;
  char *contentType;
  char *data;
  size_t size;
} ngfResHeader;

typedef struct
{
  /* data */
  char *data;
  size_t size;
} ngfResponse;


// GET response conetnt-type.
// File type
// HTML file:    text/html; charset=UTF-8
// CSS file:     text/css
// JS file:      application/javascript
// PNG file:     image/png
// JPEG file:    image/jpeg
// svg file:     image/svg+xml
// favicon file: image/vnd.microsoft.icon
char* ngf_res_conent_type(char* buf);

// Make HTML content from file.
ngfFile ngf_res_body(char* file);

// 
void ngf_res_header(ngfResHeader *header);

//
ngfResHeader make_header(size_t size, char* file);

//
ngfResHeader make_404_header(size_t size);

//
void ngf_response(ngfResponse *res, char *file);
