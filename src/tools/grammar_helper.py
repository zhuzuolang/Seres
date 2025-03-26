import os


class TokenType(object):
    Generator = 1
    Token = 2
    Literal = 3
    Separator = 4
    Placeholder = 5


class Token(object):

    def __init__(self, token_str, token_type):
        self.token_str = token_str
        self.token_type = token_type

    def __hash__(self):
        return hash(self.token_str + ":" + str(self.token_type))

    def __eq__(self, other):
        return self.token_type == other.token_type and self.token_str == other.token_str

    def __str__(self):
        if self.token_type == TokenType.Literal:
            return "\"{}\"".format(self.token_str)
        else:
            return "{}".format(self.token_str)

    def IsEmpty(self):
        return self.token_type == TokenType.Token and self.token_str == "EMPTY"

    def IsTerminal(self):
        return self.token_type == TokenType.Token or self.IsLiteral()

    def IsNonEmptyTerminal(self):
        return (self.token_type == TokenType.Token and self.token_str != "EMPTY") or self.IsLiteral()

    def IsGenerator(self):
        return self.token_type == TokenType.Generator

    def IsLiteral(self):
        return self.token_type == TokenType.Literal


TOKEN_PLACE_HOLD = Token("placeholder", TokenType.Placeholder)


class Tokenizer(object):

    def __init__(self, grammar_file):
        self.token_list = []
        with open(grammar_file, "r", encoding="utf-8") as fpr:
            for line in fpr:
                if line.strip() != "":
                    self.token_list.append(self.ParseLine(line))

    def ParseLine(self, line):
        self.current_line_index = 0
        token_list = []
        while self.current_line_index < len(line):
            current_char = line[self.current_line_index]
            if current_char.isalpha():
                token_list.append(self.ParseToken(line))
            elif current_char == '\'':
                token_list.append(self.ParseLiteral(line))
            elif current_char == '/' and line[self.current_line_index + 1] == '/':
                return token_list
            elif current_char.isspace():
                self.current_line_index += 1
            elif current_char == '|':
                token_list.append(Token('|', TokenType.Separator))
                self.current_line_index += 1
            else:
                raise NotImplementedError
        return token_list

    def ParseLiteral(self, line):
        token_str = ""
        self.current_line_index += 1
        while self.current_line_index < len(line) and line[self.current_line_index] != '\'':
            token_str += line[self.current_line_index]
            self.current_line_index += 1
        self.current_line_index += 1
        return Token(token_str, TokenType.Literal)

    def ParseToken(self, line):
        token_str = ""
        while self.current_line_index < len(line):
            current_char = line[self.current_line_index]
            if current_char.isalpha() or current_char.isdigit() or current_char == '_':
                token_str += current_char
                self.current_line_index += 1
            else:
                break

        if token_str.isupper():
            return Token(token_str, TokenType.Token)
        else:
            return Token(token_str, TokenType.Generator)

    def ParseComment(self, line):
        pass


class Reduce(object):

    def __init__(self, reduce_token_list):
        self.reduce_token_list = reduce_token_list

    def __iter__(self):
        return iter(self.reduce_token_list)


class Production(object):

    def __init__(self, token, reduce):
        self.left_token = token
        self.reduce = reduce

    def __str__(self):
        result = ""
        result += str(self.left_token)
        for reduce in self.reduce:
            result += "\n"
            result += "--->" + " ".join([str(t) for t in reduce])
        return result


class FirstSetKey(object):
    def __init__(self, left, index):
        self.left = left
        self.index = index

    def __hash__(self):
        return hash((self.left, self.index))

    def __eq__(self, other):
        return self.left == other.left and self.index == other.index

    def __str__(self):
        return "({}, {})".format(self.left, self.index)


class FirstSetValue(object):
    def __init__(self, token_list):
        self.token_tuple = tuple(token_list)

    def __hash__(self):
        return hash(self.token_tuple)

    def __eq__(self, other):
        return self.token_tuple == other.token_tuple

    def __str__(self):
        return str(self.token_tuple)

    def CountPlaceHolder(self):
        count = 0
        for token in self.token_tuple:
            if token.token_type == TokenType.Placeholder:
                count += 1
        return count


class LLK(object):

    def __init__(self, in_tokenizer):
        self.tokenizer = in_tokenizer

        self.ParseProduction()

    def ParseProduction(self):
        self.productions = {}
        index = 0
        while index < len(self.tokenizer.token_list):
            if len(self.tokenizer.token_list[index]) == 1:
                left = self.tokenizer.token_list[index][0]
                reduce = []
                index += 1
                while index < len(self.tokenizer.token_list) and len(self.tokenizer.token_list[index]) > 1:
                    reduce.append(Reduce(self.tokenizer.token_list[index][1:]))
                    index += 1
                self.productions[left] = Production(left, reduce)
            else:
                raise RuntimeError

    def GenerateFirstK(self):
        self.first_k = {}

        for left, production in self.productions.items():
            for idx, reduce in enumerate(production.reduce):
                if left not in self.first_k:
                    self.first_k[left] = dict()
                if idx not in self.first_k[left]:
                    self.first_k[left][idx] = set()
                self.first_k[left][idx].update(self.GenerateReductFirstK(reduce))

    def GenerateReductFirstK(self, reduce):
        result = set()
        for token in reduce:
            if token.IsNonEmptyTerminal():
                result.add(token)
                continue
            if token.IsGenerator():
                result = self.ExtendFirstK(result, token)

        return result

    def MergeReduceFirstK(self, left_token):
        pass

    def ExtendFirstK(self, first_k_set, token):
        pass

if __name__ == '__main__':
    tokenizer = Tokenizer("grammar.txt")
    llk = LLK(tokenizer)

    for key, production in llk.productions.items():
        print(str(production))
