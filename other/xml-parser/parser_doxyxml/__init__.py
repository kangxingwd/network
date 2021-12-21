
from . import index
from . import compound
import os

class ParserError(Exception):

    def __init__(self, error, filename):
        Exception.__init__(self, error)

        self.error = error
        self.filename = filename

    def __str__(self):
        return ("file %s: %s" % (self.filename, self.error))


class FileIOError(Exception):

    def __init__(self, error, filename):
        Exception.__init__(self, error)

        self.error = error
        self.filename = filename


class Parser(object):

    def __init__(self, cache, path):

        self.cache = cache
        self.path = path


class DoxygenIndexParser(Parser):

    def __init__(self, cache, path):
        Parser.__init__(self, cache, path)

    def parse(self):

        filename = os.path.join(self.path, "index.xml")
        try:
            # Try to get from our cache
            return self.cache[filename]
        except KeyError:

            # If that fails, parse it afresh
            try:
                result = index.parse(filename, True)
                self.cache[filename] = result
                return result
            except index.ParseError as e:
                raise ParserError(e, filename)
            except index.FileIOError as e:
                raise FileIOError(e, filename)


class DoxygenCompoundParser(Parser):

    def __init__(self, cache, path):
        Parser.__init__(self, cache, path)

    def parse(self, refid):

        filename = os.path.join(self.path,  "%s.xml" % refid)
        try:
            # Try to get from our cache
            return self.cache[filename]
        except KeyError:

            # If that fails, parse it afresh
            try:
                result = compound.parse(filename, True)
                self.cache[filename] = result
                return result
            except compound.ParseError as e:
                raise ParserError(e, filename)
            except compound.FileIOError as e:
                raise FileIOError(e, filename)

class DoxygenParserFactory(object):

    def __init__(self, path):

        self.cache = {}
        self.path = path

    def create_index_parser(self):

        return DoxygenIndexParser(self.cache, self.path)

    def create_compound_parser(self):

        return DoxygenCompoundParser(
            self.cache,
            self.path
        )
