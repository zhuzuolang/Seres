import os


class TokenType(object):
    Generator = 1
    Token = 2
    Literal = 3
    Separator = 4
    Placeholder = 5
    EMPTY = 6


class Token(object):

    def __init__(self, token_str, token_type):
        self.token_str = token_str
        self.token_type = token_type

    def __hash__(self):
        return hash((self.token_str, str(self.token_type)))

    def __eq__(self, other):
        return self.token_type == other.token_type and self.token_str == other.token_str

    def __str__(self):
        if self.token_type == TokenType.Literal:
            return "\"{}\"".format(self.token_str)
        else:
            return "{}".format(self.token_str)

    def IsEmpty(self):
        return self.token_type == TokenType.EMPTY

    def IsPlaceHolder(self):
        return self.token_type == TokenType.Placeholder

    def IsTerminal(self):
        return self.IsNonEmptyTerminal() or self.IsEmpty()

    def IsNonEmptyTerminal(self):
        return self.token_type == TokenType.Token or self.IsLiteral()

    def IsGenerator(self):
        return self.token_type == TokenType.Generator

    def IsLiteral(self):
        return self.token_type == TokenType.Literal


TOKEN_PLACE_HOLD = Token("placeholder", TokenType.Placeholder)
TOKEN_EMPTY = Token("EMPTY", TokenType.EMPTY)


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
            if token_str == "EMPTY":
                return TOKEN_EMPTY
            else:
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

    def __len__(self):
        return len(self.reduce_token_list)

    def __getitem__(self, index):
        return self.reduce_token_list[index]


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
    def __init__(self, token_list, k):
        if len(token_list) < k:
            raise RuntimeError
        token_list = token_list[0:k]
        self.token_list = token_list
        self.k = k

    def __iter__(self):
        return iter(self.token_list)

    def __hash__(self):
        return hash(tuple(self.token_list))

    def __eq__(self, other):
        return self.token_list == other.token_list

    def __str__(self):
        return "({})".format(",".join([str(t) for t in self.token_list]))

    def EmptyIndex(self):
        for idx, token in enumerate(self.token_list):
            if token.IsEmpty():
                return idx
        return -1

    def InsertToken(self, token):
        empty_index = self.EmptyIndex()
        if empty_index == -1:
            return False
        else:
            self.token_list[empty_index] = token
            return True

    def AppendValue(self, value):
        empty_index = self.EmptyIndex()
        if empty_index == -1:
            return
        else:
            for token in value:
                if token.IsNonEmptyTerminal():
                    if not self.InsertToken(token):
                        break
                else:
                    break

    def ExistPrefix(self, prefix):
        for token_p, token_this in zip(prefix.token_list, self.token_list):
            if token_p.IsTerminal() and token_this.IsTerminal():
                if token_p != token_this:
                    return False
            elif token_p.IsTerminal() and token_this.IsPlaceHolder():
                return False
            else:
                return True
        return True

    def CanAppend(self):
        empty_index = self.EmptyIndex()
        return empty_index != -1

    def ExistPlaceHolder(self):
        return self.CountPlaceHolder() > 0

    def CountPlaceHolder(self):
        count = 0
        for token in self.token_list:
            if token.token_type == TokenType.Placeholder:
                count += 1
        return count

    def PlaceHolderize(self):
        for idx, token in enumerate(self.token_list):
            if not token.IsNonEmptyTerminal():
                self.token_list[idx] = TOKEN_PLACE_HOLD

    @staticmethod
    def FillWithEmpty(token_list, k):
        empty_token_list = [TOKEN_EMPTY for _ in range(k - len(token_list))]
        token_list.extend(empty_token_list)
        return FirstSetValue(token_list, k)

    @staticmethod
    def FillWithPlaceHolder(token_list, k):
        placeholder_token_list = [TOKEN_PLACE_HOLD for _ in range(k - len(token_list))]
        token_list.extend(placeholder_token_list)
        return FirstSetValue(token_list, k)


class LLK(object):

    def __init__(self, in_tokenizer, k):
        self.tokenizer = in_tokenizer
        self.k = k

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

        def InnerGenerate():
            exist_change = False
            for left, production in self.productions.items():
                for idx, reduce in enumerate(production.reduce):
                    if left not in self.first_k:
                        self.first_k[left] = dict()
                    if idx not in self.first_k[left]:
                        self.first_k[left][idx] = set()
                    before = len(self.first_k[left][idx])
                    self.first_k[left][idx].update(self.GenerateReductFirstK(reduce))
                    after = len(self.first_k[left][idx])
                    if before != after:
                        exist_change = True
            return exist_change

        while InnerGenerate():
            continue

        self.RemovePrefix()

    def RemovePrefix(self):
        to_remove_first_k_set = {}
        for left, indexed_first_k_set in self.first_k.items():
            for idx, first_k_set in indexed_first_k_set.items():
                if left not in to_remove_first_k_set:
                    to_remove_first_k_set[left] = dict()
                if idx not in to_remove_first_k_set[left]:
                    to_remove_first_k_set[left][idx] = set()

                for i in first_k_set:
                    for j in first_k_set:
                        if i != j:
                            if i.ExistPrefix(j):
                                to_remove_first_k_set[left][idx].add(j)

        for left, indexed_first_k_set in to_remove_first_k_set.items():
            for idx, first_k_set in indexed_first_k_set.items():
                self.first_k[left][idx] = self.DifferenceSet(self.first_k[left][idx], first_k_set)

    def DifferenceSet(self, a, b):
        result = set()
        for v1 in a:
            exist = False
            for v2 in b:
                if v1 == v2:
                    exist = True
            if not exist:
                result.add(v1)
        return result

    def GenerateReductFirstK(self, reduce):
        if len(reduce) == 1 and reduce[0].token_type == TokenType.EMPTY:
            result_set = set()
            result_set.add(FirstSetValue.FillWithEmpty([], self.k))
            return result_set

        per_token_first_k_list = [set() for _ in range(len(reduce))]
        for idx, token in enumerate(reduce):
            per_token_first_k_list[idx].update(self.GenerateTokenFirstK(token))

        per_token_first_k_list = [list(s) for s in per_token_first_k_list]

        max_index_list = [len(s) - 1 for s in per_token_first_k_list]
        current_index_list = [-1 for s in per_token_first_k_list]

        def IsReachMax():
            for current_index, max_index in zip(current_index_list, max_index_list):
                if current_index < max_index:
                    return False
            return True

        def IndexEqual():
            for idx, max_index in zip(current_index_list, max_index_list):
                if idx != max_index:
                    return False
            return True

        def IndexEqual():
            for idx, max_index in zip(current_index_list, max_index_list):
                if idx != max_index:
                    return False
            return True

        def IncIndex():
            if IndexEqual():
                return

            current_inc_index = len(current_index_list) - 1
            current_index_list[current_inc_index] += 1
            while current_index_list[current_inc_index] > max_index_list[current_inc_index] and current_inc_index >= 0:
                current_index_list[current_inc_index] = 0
                current_inc_index -= 1
                if current_inc_index >= 0:
                    current_index_list[current_inc_index] += 1

        def JoinFirstK():
            current_first_k = [None for _ in range(len(per_token_first_k_list))]
            for index in range(len(per_token_first_k_list)):
                current_first_k[index] = per_token_first_k_list[index][current_index_list[index]]

            current_result_first_k = FirstSetValue.FillWithEmpty([], self.k)
            for first_k in current_first_k:
                if current_result_first_k.CanAppend():
                    current_result_first_k.AppendValue(first_k)
                    if first_k.ExistPlaceHolder():
                        break
                else:
                    break
            return current_result_first_k

        reduce_first_k_set = set()
        while not IsReachMax():
            reduce_first_k_set.add(JoinFirstK())
            IncIndex()

        for s in reduce_first_k_set:
            s.PlaceHolderize()
        return reduce_first_k_set

    def GenerateTokenFirstK(self, token):
        result_set = set()
        if token.IsNonEmptyTerminal():
            result_set.add(FirstSetValue.FillWithEmpty([token], self.k))
        elif token.IsEmpty():
            result_set.add(FirstSetValue.FillWithEmpty([], self.k))
        else:
            if token in self.first_k:
                for idx, first_k_set in self.first_k[token].items():
                    result_set.update(first_k_set)
            if len(result_set) == 0:
                result_set.add(FirstSetValue.FillWithPlaceHolder([], self.k))
        return result_set

    def MergeReduceFirstK(self, left_token):
        pass

    def ExtendFirstK(self, first_k_set, token):
        pass


if __name__ == '__main__':
    tokenizer = Tokenizer("grammar_test.txt")
    llk = LLK(tokenizer, 2)
    llk.GenerateFirstK()
    for left, first_k in llk.first_k.items():
        print(left)
        for idx, first_k_set in first_k.items():
            print(idx, [str(v) for v in first_k_set])
    # for key, production in llk.productions.items():
    #     print(str(production))
