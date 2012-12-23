#if !defined __LEXER_H__
#define __LEXER_H__

namespace energonsoftware {

class Lexer
{
public:
    enum Token
    {
        // general tokens
        OPEN_PAREN,
        CLOSE_PAREN,
        OPEN_BRACE,
        CLOSE_BRACE,

        // end of file
        END,

        // scan error
        LEX_ERROR
    };

private:
    static boost::unordered_map<std::string, int> keyword_map;

protected:
    // NOTE: this does not check for keyword overwrite
    static void set_keyword(const std::string& keyword, int token);

public:
    virtual ~Lexer() throw();

public:
    bool load(const boost::filesystem::path& filename);

    int position() const { return _current; }
    size_t length() const { return _data.length(); }
    void clear() { _current = 0; _data.erase(); }
    void reset() { _current = 0; }

    void skip_whitespace();
    bool check_token(int token);
    bool match(int token);
    bool int_literal(int& value);
    bool size_literal(size_t& value);
    bool float_literal(float& value);
    bool string_literal(std::string& value);
    bool bool_literal(bool& value);

    // swallows the rest of the current line
    void advance_line();

private:
    void skip_comments();

    char advance();
    int lex();
    bool check_keyword(int token);
    int keyword();

private:
    std::string _data;
    int _current;

protected:
    Lexer();
    explicit Lexer(const std::string& data);

private:
    DISALLOW_COPY_AND_ASSIGN(Lexer);
};

}

#endif
