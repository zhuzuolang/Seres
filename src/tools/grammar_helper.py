import os


class TokenType(object):
	Generator = 1,
	Token = 2,
	Literal = 3
	Separator = 4


class Token(object):
	
	def __init__(self, token_str, token_type):
		self.token_str = token_str
		self.token_type = token_type


class GeneratorMap(object):
	
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


class GrammarValidator(object):
	
	def __init__(self, in_generator_map):
		self.generator_map = in_generator_map
	
	def CheckLLK(self):
		pass


class GrammarGenerator(object):
	
	def __init__(self, in_generator_map):
		self.generator_map = in_generator_map
	
	def GenerateFirst(self):
		pass


if __name__ == '__main__':
	generator_map = GeneratorMap("G:/github/HybridLLVM/src/test\code1.txt")
	for token in generator_map.token_list:
		print(token[0].token_str, token[0].token_type)
	
	validator = GrammarValidator(generator_map)
	validator.CheckLLK()
