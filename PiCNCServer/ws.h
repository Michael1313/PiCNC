
char response_html[] = "HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n"
"Content-Length: ";

char response_404[] = "HTTP/1.1 404 Not Found\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n";

char response_img[] = "HTTP/1.1 200 OK\r\n"
"Content-Type: image/png; charset=UTF-8\r\n\r\n";

char response_xicon[] = "HTTP/1.1 200 OK\r\n"
"Content-Type: image/x-icon; charset=UTF-8\r\n"
"Content-Length: ";

char response_css[] = "HTTP/1.1 200 OK\r\n"
"Content-Type: text/css; charset=UTF-8\r\n\r\n";

char response_js[] = "HTTP/1.1 200 OK\r\n"
"Content-Type: text/js; charset=UTF-8\r\n\r\n";

char response_ttf[] = "HTTP/1.1 200 OK\r\n"
"Content-Type: font/ttf ; charset=UTF-8\r\n\r\n";

char response_text[] = "HTTP/1.1 200 OK\r\n"
"Content-Type: text/text; charset=UTF-8\r\n\r\n";

char response_403[] = "HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html><html><head><title>403</title>"
"<style>body { background-color: #312f2f }"
"h1 { font-size:4cm; text-align: center; color: #666;}</style></head>"
"<body><h1>403</h1></body></html>\r\n";

char GUIDKey[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"; //36

char response_ws[] = "HTTP/1.1 101 Switching Protocols\r\n"
"Upgrade: websocket\r\n"
"Connection: Upgrade\r\n"
"Sec-WebSocket-Accept: "; //97

unsigned char charset[]={"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};
