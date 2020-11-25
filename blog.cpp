#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include "../willlibcrypto/libcrypto.h"

int skip_space(std::string s, int pos_start) {
    int end = s.size() - 1;
    for(int i = pos_start; i <= end; i++) {
        if(s[i] != ' ') {
            return i;
        }
    }
    return end;
}

enum Type {
    List,
    Text,
    Pound,
    Bold,
    Italic,
    ItalicBold
};

struct Token {
    Type type;
    unsigned int start;
    unsigned int end;
};

Token next_token(std::string& s, unsigned int& pos) {
    unsigned int start = pos;

    Token token;
    if ( s[pos] == '#' ) {
        while (pos < s.size() && s[pos] == '#') {
            pos ++;
        }
        token = Token {Pound, start, pos};
    } else if ( s[pos] == '*' ){
        
        if ( s[pos + 1] == ' ' && pos == 0 ) {
            pos += 2;
            token = Token{List, start, pos};
        } else if ( s[pos + 1] == '*' ) {
            if ( s[pos + 2] == '*' ) {
                pos += 3;
                token = Token{ItalicBold, start, pos};
            } else {
                pos += 2;
                token = Token{Bold, start, pos};
            }
        } else {
            pos ++;
            token = Token{Italic, start, pos};
        }
    } else {
        while (pos < s.size() && s[pos] != '*') {
            pos ++;
        }
        //std::cout << "TEXT: " << start << " , " << pos << std::endl;
        token = Token{Text, start, pos};
    }

    return token;
}

int main(int argc, char * argv [] ) {
    if (argc < 2) {
        std::cout << "provide an input file argument";
        return 1;
    }
    std::string arg1 = std::string(argv[1]);
    std::string blog_filename_in = arg1 + ".md";
    std::string blog_filename_out = "out/" + arg1 + ".html";
    std::cout << "input file: " << blog_filename_in << "\toutput file: " << blog_filename_out << std::endl;
    std::ofstream outputFile(blog_filename_out);
    std::ostringstream outputStream{};
    std::ifstream inputFile(blog_filename_in);
    if(!inputFile.good()){
        std::cout << "input file is not good()" << std::endl;
        return 1;
    }
    if(!outputFile.good()){
        std::cout << "output file is not good()" << std::endl;
        return 1;
    }

    outputStream << "<html>" << std::endl;
    outputStream << "<head>" << std::endl;
    outputStream << "<title>Blog</title>" << std::endl;
    outputStream << "</head>" << std::endl;
    outputStream << "<body>" << std::endl;
    
    std::string buffer_input;
    bool mode_list = false;
    bool mode_bold = false;
    bool mode_italic = false;
    bool mode_italic_bold = false;
    std::string end_of_line_stack[10];
    int end_of_line_idx = 0;
    while(std::getline(inputFile, buffer_input)) {
        unsigned int i = 0;
        unsigned int length_line = buffer_input.size();
        while(i < length_line) {

            Token t = next_token(buffer_input, i);

            if (t.start == 0 && mode_list && t.type != List) {
                outputStream << "</ul>" << std::endl;
                mode_list = false;
            }


            if (t.type == Pound) {
                if (mode_list) {
                    continue;
                }
                int header_level = t.end - t.start;
                outputStream << std::string("<h").append(std::to_string(header_level)).append(">");
                end_of_line_stack[end_of_line_idx] = std::string("</h").append(std::to_string(header_level)).append(">");
                end_of_line_idx++;
                continue;
            }

            if (t.type == List) {
                if( !mode_list ) {
                    mode_list = true;
                    outputStream << "<ul>" << std::endl;
                }
                outputStream << "<li>";
                end_of_line_stack[end_of_line_idx] = "</li>";
                end_of_line_idx ++;
                continue;
            }

            if (t.type == Text) {
                if (!mode_list && !mode_bold && !mode_italic && !mode_italic_bold) {
                    outputStream << "<p>";
                    end_of_line_stack[end_of_line_idx] = "</p>";
                    end_of_line_idx ++;
                }
                outputStream << std::string_view(buffer_input.data() + t.start, t.end-t.start);
                continue;
            }

            if (t.type == Bold) {
                if (!mode_bold) {
                    mode_bold = true;
                    outputStream << "<b>";
                } else {
                    mode_bold = false;
                    outputStream << "</b>";
                }
                continue;
            }

            if (t.type == Italic) {
                if (!mode_italic) {
                    mode_italic = true;
                    outputStream << "<i>";
                } else {
                    mode_italic = false;
                    outputStream << "</i>";
                }
                continue;
            }


            if (t.type == ItalicBold) {
                if (!mode_italic_bold) {
                    mode_italic_bold = true;
                    outputStream << "<b><i>";
                } else {
                    mode_italic_bold = false;
                    outputStream << "</b></i>";
                }
                continue;
            }


            assert (false);
        }

        while(end_of_line_idx > 0) {
            end_of_line_idx--;
            outputStream << end_of_line_stack[end_of_line_idx];
        }

    }

    if(mode_list) {
        outputStream << "</ul>" << std::endl;
        mode_list = false;
    }
    

    outputStream << "<p>Checksum: ";
    outputStream << to_string(xor_checksum(outputStream.str()));
    outputStream << "</p>\n";
    outputStream << "</body>" << std::endl;
    outputStream << "</html>" << std::endl;

    outputFile << outputStream.str();

    outputFile.close();
	return 0;
}
