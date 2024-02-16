from sqllogic_parser.base_statement import BaseStatement
from sqllogic_parser.expected_result import ExpectedResult
from sqllogic_parser.token import Token
from typing import Optional, List


class Query(BaseStatement):
    def __init__(self, header: Token, line: int):
        super().__init__(header, line)
        self.label: Optional[str] = None
        self.lines: List[str] = []
        self.expected_result: Optional[ExpectedResult] = None
        self.connection_name: Optional[str] = None

    def set_label(self, label: str):
        self.label = label

    def add_lines(self, lines: List[str]):
        self.lines.extend(lines)

    def set_connection(self, connection: str):
        self.connection_name = connection

    def set_expected_result(self, expected_result: ExpectedResult):
        self.expected_result = expected_result
