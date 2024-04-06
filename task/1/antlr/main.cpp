#include "SYsULexer.h" // 确保这里的头文件名与您生成的词法分析器匹配
#include <fstream>
#include <iostream>
#include <unordered_map>

// 映射定义，将ANTLR的tokenTypeName映射到clang的格式
std::unordered_map<std::string, std::string> tokenTypeMapping = {
  { "Int", "int" },
  { "Void", "void" },
  { "Identifier", "identifier" },
  { "LeftParen", "l_paren" },
  { "RightParen", "r_paren" },
  { "RightBrace", "r_brace" },
  { "LeftBrace", "l_brace" },
  { "LeftBracket", "l_square" },
  { "RightBracket", "r_square" },
  { "Constant", "numeric_constant" },
  { "Return", "return" },
  { "Semi", "semi" },
  { "EOF", "eof" },
  { "Equal", "equal" },
  { "Plus", "plus" },
  { "Comma", "comma" },
  { "LineAfterPreprocessing", "LAP" },
  { "Whitespace", "WS" },
  { "Newline", "NL" }, 
  { "Const", "const" },
  { "Minus", "minus" },
  { "Star", "star" },
  { "Slash", "slash" },
  { "Percent", "percent" },
  { "If", "if" },
  { "While", "while" },
  { "Break", "break" },
  { "Continue", "continue" },
  { "Less", "less" },
  { "Equalequal", "equalequal" },
  { "Ampamp", "ampamp" },
  { "Lessequal", "lessequal" },
  { "Pipepipe", "pipepipe" },
  { "Exclaimequal", "exclaimequal" },
  { "Exclaim", "exclaim" },
  { "Greater", "greater"},
  { "Else", "else"},
  { "Greaterequal", "greaterequal"},



  // 在这里继续添加其他映射
};

int Line = 0;
std::string fileInfo = "";
bool startOfLine = true;
bool leadingSpace = false;

void
print_token(const antlr4::Token* token,
            const antlr4::CommonTokenStream& tokens,
            std::ofstream& outFile,
            const antlr4::Lexer& lexer)
{
  auto& vocabulary = lexer.getVocabulary();

  auto tokenTypeName =
    std::string(vocabulary.getSymbolicName(token->getType()));

  if (tokenTypeName.empty())
    tokenTypeName = "<UNKNOWN>"; // 处理可能的空字符串情况

  if (tokenTypeMapping.find(tokenTypeName) != tokenTypeMapping.end()) {
    tokenTypeName = tokenTypeMapping[tokenTypeName];
  }

  if (tokenTypeName == "LAP") {
    fileInfo = "";
    int flag = 0;
    std::string content = token->getText();

    for (std::int64_t i = 0; i < content.size(); i++)
    {

      // extract Line number
      if (content[i] == '#') {
        Line = 0;
        for (int j = i + 1; j < content.size() && content[j] != '\"'; j++) {
          if ( ! (content[j] >= '0' && content[j] <= '9') ) continue;
          Line = Line * 10 + int (content[j]) - 48;
        }
        Line -= 1;
      }

      // extract location
      if (content[i] == '\"') { flag = ( flag + 1 ) % 2; continue; }
      if ( flag == 1 ) fileInfo += content[i]; 
    }
    return;
  }

  std::int64_t pos = token->getCharPositionInLine();
  // std::string locInfo = " Loc=<0:0>";

  if (tokenTypeName == "WS") {
    leadingSpace = true;
    return;
  }

  if (tokenTypeName == "NL") {
    startOfLine = true;
    Line += 1;
    return;
  }

  if (token->getText() != "<EOF>")
    outFile << tokenTypeName << " '" << token->getText() << "'";
  else
    outFile << tokenTypeName << " '"
            << "'";
  if (startOfLine) {
    outFile << "\t [StartOfLine]";
    startOfLine = false;
  }
  if (leadingSpace) {
    outFile << " [LeadingSpace]";
    leadingSpace = false;
  }

  outFile << " Loc=<" << fileInfo << ":" << Line << ":" << pos + 1 << ">" << std::endl;
}

int
main(int argc, char* argv[])
{
  if (argc != 3) {
    std::cout << "Usage: " << argv[0] << " <input> <output>\n";
    return -1;
  }

  std::ifstream inFile(argv[1]);
  if (!inFile) {
    std::cout << "Error: unable to open input file: " << argv[1] << '\n';
    return -2;
  }

  std::ofstream outFile(argv[2]);
  if (!outFile) {
    std::cout << "Error: unable to open output file: " << argv[2] << '\n';
    return -3;
  }

  std::cout << "程序 '" << argv[0] << std::endl;
  std::cout << "输入 '" << argv[1] << std::endl;
  std::cout << "输出 '" << argv[2] << std::endl;

  antlr4::ANTLRInputStream input(inFile);
  SYsULexer lexer(&input);

  antlr4::CommonTokenStream tokens(&lexer);
  tokens.fill();

  for (auto&& token : tokens.getTokens()) {
    print_token(token, tokens, outFile, lexer);
  }
}
