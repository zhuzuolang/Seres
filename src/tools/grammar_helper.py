import os


class TokenType(object):
	Generator = 1
	Token = 2
	Literal = 3
	Separator = 4


class Token(object):
	
	def __init__(self, token_str, token_type):
		self.token_str = token_str
		self.token_type = token_type
	
	def __hash__(self):
		return hash(self.token_str + ":" + str(self.token_type))
	
	def __eq__(self, other):
		return self.token_type == other.token_type and self.token_str == other.token_str
	
	def __str__(self):
		return "{}:{}".format(self.token_str, self.token_type)
	
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


class Tokenizer(object):
	
	def __init__(self, grammar_file):
		self.token_list = []
		with open(grammar_file, "r") as fpr:
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


class Production(object):
	
	def __init__(self, left, reduce_list):
		self.left = left
		self.reduce = reduce_list
	
	def __str__(self):
		return "{}:{}".format(self.left.token_str, self.reduce)


class GrammarValidator(object):
	
	def __init__(self, in_tokenizer):
		self.tokenizer = in_tokenizer
		self.productions = {}
		
		index = 0
		while index < len(self.tokenizer.token_list):
			if len(self.tokenizer.token_list[index]) == 1:
				left = self.tokenizer.token_list[index][0]
				reduce = []
				index += 1
				while index < len(self.tokenizer.token_list) and len(self.tokenizer.token_list[index]) > 1:
					reduce.append(self.tokenizer.token_list[index][1:])
					index += 1
				self.productions[left] = Production(left, reduce)
			else:
				raise RuntimeError
		
		self.first_map = dict()
		self.GenerateProductionFirst()
	
	def GenerateProductionFirst(self):
		self.first_map = dict()
		
		contain_changed = True
		while contain_changed:
			contain_changed = False
			for left, production in self.productions.items():
				if left not in self.first_map:
					self.first_map[left] = [set() for i in range(len(production.reduce))]
					contain_changed = True
				
				for idx, reduce in enumerate(production.reduce):
					new_set = self.GenerateReduceFirst(reduce)
					old_set_count = len(self.first_map[left][idx])
					self.first_map[left][idx].update(new_set)
					new_set_count = len(self.first_map[left][idx])
					
					if new_set_count > old_set_count:
						contain_changed = True
		
		for key, value in self.first_map.items():
			for list in value:
				print(key.token_str, "->", [token.token_str for token in list])
	
	def ExistEmpty(self, token_set):
		for token in token_set:
			if token.IsEmpty():
				return True
		return False
	
	def GenerateReduceFirst(self, reduce):
		first_set = set()
		contain_empty_count = 0
		
		for idx, token in enumerate(reduce):
			current_reduce_contain_empty = False
			if token.IsNonEmptyTerminal():
				first_set.add(token)
				break
			
			if token.IsEmpty():
				current_reduce_contain_empty = True
				first_set.add(token)
				contain_empty_count += 1
				continue
			
			if token.IsGenerator():
				exist_empty = False
				for reduce_first_set in self.first_map.get(token, []):
					first_set.update(reduce_first_set)
					if self.ExistEmpty(reduce_first_set):
						exist_empty = True
				if exist_empty:
					contain_empty_count += 1
					current_reduce_contain_empty = True
			
			if not current_reduce_contain_empty:
				break
		if contain_empty_count >= len(reduce):
			return first_set
		else:
			new_set = set()
			for token in first_set:
				if not token.IsEmpty():
					new_set.add(token)
			return new_set
	
	def CheckLLK(self):
		pass


if __name__ == '__main__':
	tokenizer = Tokenizer("G:/github/HybridLLVM/src/test\code1.txt")
	# for token in tokenizer.token_list:
	# 	print([str(t) for t in token])
	
	validator = GrammarValidator(tokenizer)
	validator.CheckLLK()
