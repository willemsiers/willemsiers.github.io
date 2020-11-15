#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

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
    int start;
    int end;
};

Token next_token(std::string& s, int& pos) {
    int start = pos;

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
    bool mode_bold = false;
    bool mode_italic = false;
    bool mode_italic_bold = false;
    std::string end_of_line_stack[10];
    int end_of_line_idx = 0;
    while(std::getline(inputFile, buffer_input)) {
        int i = 0;
        int length_line = buffer_input.size();
        while(i < length_line) {

            Token t = next_token(buffer_input, i);

            if (t.start == 0 && mode_list && t.type != List) {
                outputFile << "</ul>" << std::endl;
                mode_list = false;
            }


            if (t.type == Pound) {
                if (mode_list) {
                    continue;
                }
                int header_level = t.end - t.start;
                outputFile << std::string("<h").append(std::to_string(header_level)).append(">");
                end_of_line_stack[end_of_line_idx] = std::string("</h").append(std::to_string(header_level)).append(">");
                end_of_line_idx++;
                continue;
            }

            if (t.type == List) {
                if( !mode_list ) {
                    mode_list = true;
                    outputFile << "<ul>" << std::endl;
                }
                outputFile << "<li>";
                end_of_line_stack[end_of_line_idx] = "</li>";
                end_of_line_idx ++;
                continue;
            }

            if (t.type == Text) {
                if (!mode_list && !mode_bold && !mode_italic && !mode_italic_bold) {
                    outputFile << "<p>";
                    end_of_line_stack[end_of_line_idx] = "</p>";
                    end_of_line_idx ++;
                }
                outputFile << std::string_view(buffer_input.data() + t.start, t.end-t.start);
                continue;
            }

            if (t.type == Bold) {
                if (!mode_bold) {
                    mode_bold = true;
                    outputFile << "<b>";
                } else {
                    mode_bold = false;
                    outputFile << "</b>";
                }
                continue;
            }

            if (t.type == Italic) {
                if (!mode_italic) {
                    mode_italic = true;
                    outputFile << "<i>";
                } else {
                    mode_italic = false;
                    outputFile << "</i>";
                }
                continue;
            }


            if (t.type == ItalicBold) {
                if (!mode_italic_bold) {
                    mode_italic_bold = true;
                    outputFile << "<b><i>";
                } else {
                    mode_italic_bold = false;
                    outputFile << "</b></i>";
                }
                continue;
            }


            assert (false);
        }

        while(end_of_line_idx > 0) {
            end_of_line_idx--;
            outputFile << end_of_line_stack[end_of_line_idx];
        }

    }

    if(mode_list) {
        outputFile << "</ul>" << std::endl;
        mode_list = false;
    }
    

    outputFile << "</body>" << std::endl;
    outputFile << "</html>" << std::endl;

    outputFile.close();
	return 0;
}
