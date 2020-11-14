#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>

int skip_space(std::string s, int pos_start) {
    int end = s.size() - 1;
    for(int i = pos_start; i <= end; i++) {
        if(s[i] != ' ') {
            return i;
        }
    }
    return end;
}

int main() {
    char blog_filename_out[] = "out_blog0001.html";
    char blog_filename_in[] = "blog_entry_0001.md";
    std::ofstream outputFile(blog_filename_out);
    std::ifstream inputFile(blog_filename_in);

    outputFile << "<html>" << std::endl;
    outputFile << "<head>" << std::endl;
    outputFile << "<title>Blog</title>" << std::endl;
    outputFile << "</head>" << std::endl;
    outputFile << "<body>" << std::endl;
    
    std::string buffer_input;
    bool mode_list = false;
    while(std::getline(inputFile, buffer_input)) {
        int i = 0;
        int length_line = buffer_input.size();
        while(i < length_line) {

            char c = buffer_input[i];

            if( c == '*' ) {
                if( buffer_input[i + 1] == ' ' ) {
                    i++; // consume char
                    if(!mode_list) {
                        mode_list = true;
                        outputFile << "<ul>" << std::endl;
                    }
                }
            } else {
                if(mode_list) {
                    outputFile << "</ul>" << std::endl;
                    mode_list = false;
                }
            }

            if (mode_list) {
                outputFile << "<li>";
                i++; // consume char
                i = skip_space(buffer_input, i);
                outputFile << std::string_view(buffer_input.data() + i);
                outputFile << "</li>" << std::endl;
                i = length_line - 1; // consume line
            } else if ( c == '#') {
                if ( buffer_input[i + 1] == '#') {
                    outputFile << "<h2>";
                    i++; // consume char
                    i++; // consume char
                    i = skip_space(buffer_input, i);
                    outputFile << std::string_view(buffer_input.data() + i);
                    outputFile << "</h2>" << std::endl;
                    i = length_line - 1; // consume line
                } else {
                    outputFile << "<h1>";
                    i++; // consume char
                    i = skip_space(buffer_input, i);
                    outputFile << std::string_view(buffer_input.data() + i);
                    outputFile << "</h1>" << std::endl;
                    i = length_line - 1; // consume line
                }
            } else {
                outputFile << "<p>";
                i = skip_space(buffer_input, i);
                outputFile << std::string_view(buffer_input.data() + i);
                outputFile << "</p>" << std::endl;
                i = length_line - 1; // consume line
            }

            i++; // consume char
        }
    }
    

    outputFile << "</body>" << std::endl;
    outputFile << "</html>" << std::endl;

    outputFile.close();
	return 0;
}
