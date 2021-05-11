#include <string>
std::string base64_encode(std::string const& s, bool url);
std::string base64_encode_pem(std::string const& s);
std::string base64_encode_mime(std::string const& s);

std::string base64_decode(std::string const& s, bool remove_linebreaks);
std::string base64_encode(unsigned char const*, size_t len, bool url);