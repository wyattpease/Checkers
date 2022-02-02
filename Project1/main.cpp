#include <string>
#include <fstream>
#include <vector>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <map>
#include <tuple>

using namespace std;

map<int, string> board_row = { { 0, "8" }, { 1, "7" }, { 2, "6" }, { 3, "5" }, { 4, "4" }, { 5, "3" }, { 6, "2" }, { 7, "1" } };
map<int, string> board_col = { { 0, "a" }, { 1, "b" }, { 2, "c" }, { 3, "d" }, { 4, "e" }, { 5, "f" }, { 6, "g" }, { 7, "h" } };

class Checker {
	public:
		int row;
		int col;
		bool isKing;
		int id;
		char piece;
		Checker(int r, int c, bool K, int ID, char p) {
			row = r;
			col = c;
			isKing = K;
			id = ID;
			piece = p;
		};
		Checker(char p) {
			piece = p;
		};
		Checker() = default;
};

class Player {
	public:
		int turn;
		int type;
		char color;
		char adv_color;
		int piece_count;
		int king_count;
		int adv_count;
		int adv_king_count;
		string self_king_row;
		string adv_king_row;
		float time;
		int limit;
		vector<Checker> board[8];
		map<int, Checker> self;
		map<int, Checker> adversary;
		string move;
		bool isJump;
		int movesAvail;
		Player() = default;
};

typedef void (*fp_t)();
//vector<fp_t> v;
//return v;

Player check_downright(Player p, Checker c_start, bool* jAvail, bool isMax) {
	if ((c_start.row + 1 < 8) && (c_start.col + 1 < 8)) {
		char opponent;
		Checker c;
		if (isMax)
		{
			opponent = p.adv_color;
			c = p.self[c_start.id];
		}
		else
		{
			opponent = p.color;
			c = p.adversary[c_start.id];
		}

		Checker dr = p.board[c.row + 1][c.col + 1];

		if (dr.piece == opponent|| dr.piece == toupper(opponent)) {
			if ((c.row + 2 < 8) && (c.col + 2 < 8) && (p.board[c.row + 2][c.col + 2].piece == '.')) {
				string pos1 = board_col[c.col] + board_row[c.row];
				if (isMax)
				{
					if (dr.isKing)
						p.adv_king_count--;
					else
						p.adv_count--;
					p.adversary.erase(dr.id);
				}
				else
				{
					if (dr.isKing)
						p.king_count--;
					else
						p.piece_count--;
					p.self.erase(dr.id);
				}
				swap(p.board[c.row + 2][c.col + 2], p.board[c.row][c.col]);				
				p.board[c.row + 1][c.col + 1] = p.board[c.row][c.col];		
				c.row = c.row + 2;
				c.col = c.col + 2;
				string pos2 = board_col[c.col] + board_row[c.row];
				p.move = "J " + pos1 + " " + pos2;
				*jAvail = true;
				p.isJump = true;	
				p.board[c.row][c.col] = c;
				if (isMax)
					p.self[c.id] = c;
				else
					p.adversary[c.id] = c;
				return p;
			}
		}
		else if (dr.piece == '.' && !*jAvail) {
			string pos1 = board_col[c.col] + board_row[c.row];
			swap(p.board[c.row + 1][c.col + 1], p.board[c.row][c.col]);
			c.row = c.row + 1;
			c.col = c.col + 1;
			string pos2 = board_col[c.col] + board_row[c.row];
			p.move = "E " + pos1 + " " + pos2;
			p.isJump = false;
			p.board[c.row][c.col] = c;
			if (isMax)
				p.self[c.id] = c;
			else
				p.adversary[c.id] = c;
			return p;
		}
	}
	return Player();
}

Player check_downleft(Player p, Checker c_start, bool * jAvail, bool isMax) {
	if ((c_start.row + 1 < 8) && (c_start.col - 1 > -1)) {
		Checker dl = p.board[c_start.row + 1][c_start.col - 1];
		char opponent;
		Checker c;
		if (isMax)
		{
			opponent = p.adv_color;
			c = p.self[c_start.id];
		}
		else
		{
			opponent = p.color;
			c = p.adversary[c_start.id];
		}
		if (dl.piece == opponent || dl.piece == toupper(opponent)) {
			if ((c.row + 2 < 8) && (c.col - 2 > -1) && (p.board[c.row + 2][c.col - 2].piece == '.')) {
				if (isMax)
				{
					if (dl.isKing)
						p.adv_king_count--;
					else
						p.adv_count--;
					p.adversary.erase(dl.id);
				}
				else
				{
					if (dl.isKing)
						p.king_count--;
					else
						p.piece_count--;
					p.self.erase(dl.id);
				}
				string pos1 = board_col[c.col] + board_row[c.row];
				swap(p.board[c.row + 2][c.col - 2], p.board[c.row][c.col]);				
				p.board[c.row + 1][c.col - 1] = p.board[c.row][c.col];
				c.row = c.row + 2;
				c.col = c.col - 2;
				string pos2 = board_col[c.col] + board_row[c.row];
				p.board[c.row][c.col] = c;
				if (isMax)
					p.self[c.id] = c;
				else
					p.adversary[c.id] = c;
				
				p.move = "J " + pos1 + " " + pos2;
				*jAvail = true;
				p.isJump = true;
				return p;
			}
		}
		else if (dl.piece == '.' && !*jAvail) {
			string pos1 = board_col[c.col] + board_row[c.row];
			swap(p.board[c.row + 1][c.col - 1], p.board[c.row][c.col]);
			c.row = c.row + 1;
			c.col = c.col - 1;
			p.board[c.row][c.col] = c;
			if (isMax)
				p.self[c.id] = c;
			else
				p.adversary[c.id] = c;
			string pos2 = board_col[c.col] + board_row[c.row];
			p.move = "E " + pos1 + " " + pos2;
			p.isJump = false;
			return p;
		}
	}
	return Player();
}

Player check_upright(Player p, Checker c_start, bool * jAvail, bool isMax) {
	if ((c_start.row - 1 > -1) && (c_start.col + 1 < 8)) {
		Checker ur = p.board[c_start.row - 1][c_start.col + 1];

		char opponent;
		Checker c;
		if (isMax)
		{
			opponent = p.adv_color;
			c = p.self[c_start.id];
		}
		else
		{
			opponent = p.color;
			c = p.adversary[c_start.id];
		}

		if (ur.piece == opponent || ur.piece == toupper(opponent)) {
			if ((c.row - 2 > -1) && (c.col + 2 < 8) && (p.board[c.row - 2][c.col + 2].piece == '.')) {
				string pos1 = board_col[c.col] + board_row[c.row];
				if (isMax)
				{
					if (ur.isKing)
						p.adv_king_count--;
					else
						p.adv_count--;
					p.adversary.erase(ur.id);
				}
				else
				{
					if (ur.isKing)
						p.king_count--;
					else
						p.piece_count--;
					p.self.erase(ur.id);
				}

				swap(p.board[c.row - 2][c.col + 2], p.board[c.row][c.col]);				
				p.board[c.row - 1][c.col + 1] = p.board[c.row][c.col];
				c.row = c.row - 2;
				c.col = c.col + 2;
				string pos2 = board_col[c.col] + board_row[c.row];
				p.board[c.row][c.col] = c;
				p.move = "J " + pos1 + " " + pos2;
				*jAvail = true;
				p.isJump = true;
				if (isMax)
					p.self[c.id] = c;
				else
					p.adversary[c.id] = c;
				return p;
			}
		}
		else if (ur.piece == '.' && !*jAvail) {
			string pos1 = board_col[c.col] + board_row[c.row];
			swap(p.board[c.row - 1][c.col + 1], p.board[c.row][c.col]);
			c.row = c.row - 1;
			c.col = c.col + 1;
			string pos2 = board_col[c.col] + board_row[c.row];
			p.board[c.row][c.col] = c;
			p.move = "E " + pos1 + " " + pos2;
			p.isJump = false;
			if (isMax)
				p.self[c.id] = c;
			else
				p.adversary[c.id] = c;
			return p;
		}
	}
	return Player();
}

Player check_upleft(Player p, Checker c_start, bool * jAvail, bool isMax) {
	char opponent;
	Checker c;
	if (isMax)
	{
		opponent = p.adv_color;
		c = p.self[c_start.id];
	}
	else
	{
		opponent = p.color;
		c = p.adversary[c_start.id];
	}

	if ((c.row - 1 > -1) && (c.col - 1 > -1)) {
		Checker ul = p.board[c.row - 1][c.col - 1];		
		if (ul.piece == opponent || ul.piece == toupper(opponent)) {
			if ((c.row - 2 > -1) && (c.col - 2 > -1) && (p.board[c.row - 2][c.col - 2].piece == '.')) {
				string pos1 = board_col[c.col] + board_row[c.row];
				if (isMax)
				{
					if (ul.isKing)
						p.adv_king_count--;
					else
						p.adv_count--;
					p.adversary.erase(ul.id);
				}
				else
				{
					if (ul.isKing)
						p.king_count--;
					else
						p.piece_count--;
					p.self.erase(ul.id);
				}

				swap(p.board[c.row - 2][c.col - 2], p.board[c.row][c.col]);				
				p.board[c.row - 1][c.col - 1] = p.board[c.row][c.col];
				c.row = c.row - 2;
				c.col = c.col - 2;
				string pos2 = board_col[c.col] + board_row[c.row];
				p.move = "J " + pos1 + " " + pos2;
				p.isJump = true;
				*jAvail = true;
				p.board[c.row][c.col] = c;
				if (isMax)
					p.self[c.id] = c;
				else
					p.adversary[c.id] = c;
				return p;
			}
		}
		else if (ul.piece == '.' && !*jAvail) {
			string pos1 = board_col[c.col] + board_row[c.row];
			swap(p.board[c.row - 1][c.col - 1], p.board[c.row][c.col]);
			c.row = c.row - 1;
			c.col = c.col - 1;
			string pos2 = board_col[c.col] + board_row[c.row];
			p.move = "E " + pos1 + " " + pos2;
			p.isJump = false;
			p.board[c.row][c.col] = c;
			if (isMax)
				p.self[c.id] = c;
			else
				p.adversary[c.id] = c;
			return p;
		}
	}
	return Player();
}

Player king_me(Player p, Checker c, bool isMax) {
	if (!isMax)
	{
		if (board_row[c.row] == p.adv_king_row && !c.isKing)
		{
			c.piece = toupper(c.piece);
			c.isKing = true;
			p.adversary[c.id] = c;
			p.board[c.row][c.col] = c;
			p.adv_count--;
			p.adv_king_count++;
		}
	}
	else
	{
		if (board_row[c.row] == p.self_king_row && !c.isKing)
		{
			c.piece = toupper(c.piece);
			c.isKing = true;
			p.self[c.id] = c;
			p.board[c.row][c.col] = c;
			p.piece_count--;
			p.king_count++;
		}
	}

	return p;
}


vector<Player> check_jumps(Player p, Checker c, bool* jAvail, bool isMax) {
	Player temp;
	vector<Player> vp;
	vector<Player> vpj;
	vector<Player> f;

	if (!isMax)
	{
		if (p.adv_color == 'b' || c.isKing) {
			temp = check_downleft(Player() = p, c, jAvail, isMax);
			if (!temp.board->empty())
				vp.push_back(temp);
			temp = check_downright(Player() = p, c, jAvail, isMax);
			if (!temp.board->empty())
				vp.push_back(temp);
		}

		if (p.adv_color == 'w' || c.isKing)
		{
			temp = check_upleft(Player() = p, c, jAvail, isMax);
			if (!temp.board->empty())
				vp.push_back(temp);
			temp = check_upright(Player() = p, c, jAvail, isMax);
			if (!temp.board->empty())
				vp.push_back(temp);
		}
	}
	else
	{
		Player temp;
		if (p.color == 'b' || c.isKing) {
			temp = check_downleft(Player() = p, c, jAvail, isMax);
			if (!temp.board->empty())
				vp.push_back(temp);
			temp = check_downright(Player() = p, c, jAvail, isMax);
			if (!temp.board->empty())
				vp.push_back(temp);
		}

		if (p.color == 'w' || c.isKing)
		{
			temp = check_upleft(Player() = p, c, jAvail, isMax);
			if (!temp.board->empty())
				vp.push_back(temp);
			temp = check_upright(Player() = p, c, jAvail, isMax);
			if (!temp.board->empty())
				vp.push_back(temp);
		}
	}
	//only returning one jump, need to return all possible jumps
	if (vp.empty())
	{
		p = king_me(p, c, isMax);
		vp.push_back(p);
		return vp;
	}
	else {
		for (Player a : vp)
		{
			a.move = p.move + '\n' + a.move;
			if (isMax) {
				vector<Player> vp1 = check_jumps(a, a.self[c.id], jAvail, isMax);
				f.insert(f.begin(), vp1.begin(), vp1.end());
			}
			else
			{
				vector<Player> vp1 = check_jumps(a, a.adversary[c.id], jAvail, isMax);
				f.insert(f.begin(), vp1.begin(), vp1.end());
			}
		}

		return f;
	}
}

vector<Player> actions(Player p, bool isMax) {
	vector<Player> vp;
	vector<Player> vpj;
	vector<Player> temp_v;
	Player temp;
	bool* jAvail = new bool;
	*jAvail = false;
	if (!isMax) {
		for (auto c = p.adversary.begin(); c != p.adversary.end(); ++c) {
			if (p.adv_color == 'b' || c->second.isKing) {

				temp = check_downleft(Player() = p, c->second, jAvail, isMax);
				if (!temp.board->empty()) {
					if (!temp.isJump)
					{
						temp = king_me(temp, temp.adversary[c->second.id], isMax);
						vp.push_back(temp);
					}
					else 
					{
						temp_v = check_jumps(temp, temp.adversary[c->second.id], jAvail, isMax);
						vpj.insert(vpj.begin(), temp_v.begin(), temp_v.end());
					}
				}
				
				temp = check_downright(Player() = p, c->second, jAvail, isMax);
				if (!temp.board->empty()) {
					if (!temp.isJump)
					{
						temp = king_me(temp, temp.adversary[c->second.id], isMax);
						vp.push_back(temp);
					}
					else 
					{
						temp_v = check_jumps(temp, temp.adversary[c->second.id], jAvail, isMax);
						vpj.insert(vpj.begin(), temp_v.begin(), temp_v.end());
					}
				}
			}

			if (p.adv_color == 'w' || c->second.isKing)
			{
				temp = check_upleft(Player() = p, c->second, jAvail, isMax);
				if (!temp.board->empty()) {
					if (!temp.isJump)
					{
						temp = king_me(temp, temp.adversary[c->second.id], isMax);
						vp.push_back(temp);
					}
					else 
					{
						temp_v = check_jumps(temp, temp.adversary[c->second.id], jAvail, isMax);
						vpj.insert(vpj.begin(), temp_v.begin(), temp_v.end());
					}
				}

				temp = check_upright(Player() = p, c->second, jAvail, isMax);
				if (!temp.board->empty()) {
					if (!temp.isJump)
					{
						temp = king_me(temp, temp.adversary[c->second.id], isMax);
						vp.push_back(temp);
					}
					else
					{
						temp_v = check_jumps(temp, temp.adversary[c->second.id], jAvail, isMax);
						vpj.insert(vpj.begin(), temp_v.begin(), temp_v.end());
					}
				}
			}
		}
	}
	else
	{
		for (auto c = p.self.begin(); c != p.self.end(); ++c) {			
			if (p.color == 'b' || c->second.isKing) {

				temp = check_downleft(Player() = p, c->second, jAvail, isMax);
				if (!temp.board->empty()) {
					if (!temp.isJump)
					{
						temp = king_me(temp, temp.self[c->second.id], isMax);
						vp.push_back(temp);
					}
					else 
					{
						temp_v = check_jumps(temp, temp.self[c->second.id], jAvail, isMax);
						vpj.insert(vpj.begin(), temp_v.begin(), temp_v.end());
					}
				}
					
				temp = check_downright(Player() = p, c->second, jAvail, isMax);
				if (!temp.board->empty()) {
					if (!temp.isJump)
					{
						temp = king_me(temp, temp.self[c->second.id], isMax);
						vp.push_back(temp);
					}
					else 
					{
						temp_v = check_jumps(temp, temp.self[c->second.id], jAvail, isMax);
						vpj.insert(vpj.begin(), temp_v.begin(), temp_v.end());
					}
				}
			}

			if (p.color == 'w' || c->second.isKing)
			{
				temp = check_upleft(Player() = p, c->second, jAvail, isMax);
				if (!temp.board->empty()) {
					if (!temp.isJump)
					{
						temp = king_me(temp, temp.self[c->second.id], isMax);
						vp.push_back(temp);
					}
					else 
					{
						temp_v = check_jumps(temp, temp.self[c->second.id], jAvail, isMax);
						vpj.insert(vpj.begin(), temp_v.begin(), temp_v.end());
					}
				}

				temp = check_upright(Player() = p, c->second, jAvail, isMax);
				if (!temp.board->empty()) {
					if (!temp.isJump)
					{
						temp = king_me(temp, temp.self[c->second.id], isMax);
						vp.push_back(temp);
					}
					else
					{
						temp_v = check_jumps(temp, temp.self[c->second.id], jAvail, isMax);
						vpj.insert(vpj.begin(), temp_v.begin(), temp_v.end());
					}
				}
			}
		}
	}
	if (*jAvail)
	{
		delete jAvail;
		return vpj;
	}

	
	delete jAvail;
	return vp;
}


Player Get_Player() {
	Player p;
	ifstream file("input.txt");
	string line;
	getline(file, line);
	if (line == "SINGLE")
		p.type = 0;
	else
		p.type = 1;
	getline(file, line);
	if (line == "BLACK")
	{
		p.adv_color = 'w';
		p.color = 'b';
		p.adv_king_row = "8";
		p.self_king_row = "1";
	}
	else {
		p.adv_color = 'b';
		p.color = 'w';
		p.adv_king_row = "1";
		p.self_king_row = "8";
	}
	getline(file, line);
	p.time = stof(line);

	map<int, Checker> b;
	map<int, Checker> w;
	int b_id = 0;
	int w_id = 0;
	p.piece_count = 0;
	p.king_count = 0;
	p.adv_count = 0;
	p.adv_king_count = 0;
	if (p.color == 'b') {
		for (int i = 0; i < 8; i++) {
			vector<Checker> result;
			getline(file, line);
			int u = 0;
			for (char ch : line)
			{
				if (ch == 'b') {
					Checker c = Checker(i, u, false, b_id, 'b');
					p.self.insert(std::pair<int, Checker>(b_id, c));
					p.piece_count++;
					result.push_back(c);
					b_id++;
				}
				else if (ch == 'B') {
					Checker c =  Checker(i, u, true, b_id, 'B');
					p.self.insert(std::pair<int, Checker>(b_id, c));
					p.king_count++;
					result.push_back(c);
					b_id++;
				}
				else if (ch == 'w') {
					Checker c = Checker(i, u, false, w_id, 'w');
					p.adversary.insert(std::pair<int, Checker>(w_id, c));
					p.adv_count++;
					result.push_back(c);
					w_id++;
				}
				else if (ch == 'W') {
					Checker c = Checker(i, u, true, w_id, 'W');
					p.adversary.insert(std::pair<int, Checker>(w_id, c));
					p.adv_king_count++;
					result.push_back(c);
					w_id++;
				}
				else {
					result.push_back(Checker('.'));
				}
				u++;
			}
			p.board[i] = result;
		}
	}
	else {
		for (int i = 0; i < 8; i++) {
			vector<Checker> result;
			getline(file, line);
			int u = 0;
			for (char ch : line)
			{
				if (ch == 'b') {
					Checker c = Checker(i, u, false, b_id, 'b');
					p.adversary.insert(std::pair<int, Checker>(b_id, c));
					p.adv_count++;
					result.push_back(c);
					b_id++;
				}
				else if (ch == 'B') {
					Checker c = Checker(i, u, true, b_id, 'B');
					p.adversary.insert(std::pair<int, Checker>(b_id, c));
					p.adv_king_count++;
					result.push_back(c);
					b_id++;
				}
				else if (ch == 'w') {
					Checker c = Checker(i, u, false, w_id, 'w');
					p.self.insert(std::pair<int, Checker>(w_id, c));
					result.push_back(c);
					p.piece_count++;
					w_id++;
				}
				else if (ch == 'W') {
					Checker c = Checker(i, u, true, w_id, 'W');
					p.self.insert(std::pair<int, Checker>(w_id, c));
					result.push_back(c);
					p.king_count++;
					w_id++;
				}
				else {
					result.push_back(Checker('.'));
				}
				u++;
			}
			p.board[i] = result;
		}
	}
	return p;
}

bool terminal_test(Player p) {
	return (p.adversary.empty() || p.self.empty());
		//return true;
	/*else if (moves.empty())
	{
		*Empty = true;
		return true;
	}*/
}

int get_threatened(Player& lhp, Player& rhp) {
	int lpc = lhp.piece_count + (lhp.king_count * 2);
	int rpc = rhp.piece_count + (rhp.king_count * 2);
	return lpc < rpc;
}

int get_adv_threatened(Player& lhp, Player& rhp) {
	int lpc = lhp.adv_count + (lhp.adv_king_count * 2);
	int rpc = rhp.adv_count + (rhp.adv_king_count * 2);
	return lpc < rpc;
}

double h_val(Player p, bool isMax) {
	int count = (p.piece_count - p.adv_count) * 3;
	int k_count = (p.king_count - p.adv_king_count) * 5;

	int threatened_pieces = 0;
	int adv_threatened_pieces = 0;
	int center_pieces = 0;
	int adv_center_pieces = 0;
	int king_row_pieces = 0;
	int adv_king_row_pieces = 0;
	int mid_row_pieces = 0;
	int total = 0;
	/*if (*noMoves)
	{
		delete noMoves;
		if (isMax)
			return -10000;
		else 
			return 10000;
	}*/
	if (p.adversary.empty())
		return 10000;

	if (p.self.empty())
		return -10000;

	for (auto c = p.self.begin(); c != p.self.end(); ++c) {
		if (p.color == 'b')
		{
			if ((c->second.row - 1 > -1) && (c->second.col - 1 > -1))
			{
				if (p.board[c->second.row - 1][c->second.col - 1].piece == '.')
					threatened_pieces++;
			}
			if ((c->second.row - 1 > -1) && (c->second.col + 1 <= 7))
			{
				if (p.board[c->second.row - 1][c->second.col + 1].piece == '.')
					threatened_pieces++;
			}

		}
		else
		{
			if ((c->second.row + 1 < 8) && (c->second.col - 1 > -1))
			{
				if (p.board[c->second.row + 1][c->second.col - 1].piece == '.')
					threatened_pieces++;
			}

			if ((c->second.row + 1 < 8) && (c->second.col + 1 < 8))
			{
				if (p.board[c->second.row + 1][c->second.col + 1].piece == '.')
					threatened_pieces++;
			}
		}
	}

	/*for (auto c = p.adversary.begin(); c != p.adversary.end(); ++c) {
		if (p.adv_color == 'b')
		{
			if ((c->second.row - 1 > -1) && (c->second.col - 1 > -1))
			{
				if (p.board[c->second.row - 1][c->second.col - 1].piece == '.')
					adv_threatened_pieces++;
			}
			if ((c->second.row - 1 > -1) && (c->second.col + 1 <= 7))
			{
				if (p.board[c->second.row - 1][c->second.col + 1].piece == '.')
					adv_threatened_pieces++;
			}

		}
		else
		{
			if ((c->second.row + 1 < 8) && (c->second.col - 1 > -1))
			{
				if (p.board[c->second.row + 1][c->second.col - 1].piece == '.')
					adv_threatened_pieces++;
			}

			if ((c->second.row + 1 < 8) && (c->second.col + 1 < 8))
			{
				if (p.board[c->second.row + 1][c->second.col + 1].piece == '.')
					adv_threatened_pieces++;
			}
		}*/
	/*vector<Player> vp = actions(p, !isMax);
	if (!vp.empty())
	{
		
		if (isMax)
		{
			auto min_it = std::min_element(vp.begin(), vp.end(), get_threatened);
			threatened_pieces = p.piece_count - min_it->piece_count; 
			threatened_kings = p.king_count - min_it->king_count;
		}
		else
		{
			auto min_it = std::min_element(vp.begin(), vp.end(), get_adv_threatened);
			threatened_pieces = p.adv_count - min_it->adv_count;
			threatened_kings = p.adv_king_count - min_it->adv_king_count;
		}
	}*/
	if (p.turn < 30)
	{
		if (p.color == 'b')
		{
			if (p.board[0][1].piece == 'b')
				king_row_pieces++;
			if (p.board[0][3].piece == 'b')
				king_row_pieces++;
			if (p.board[0][5].piece == 'b')
				king_row_pieces++;
			if (p.board[0][7].piece == 'b')
				king_row_pieces++;

			if (p.board[7][1].piece == 'w')
				adv_king_row_pieces++;
			if (p.board[7][3].piece == 'w')
				adv_king_row_pieces++;
			if (p.board[7][5].piece == 'w')
				adv_king_row_pieces++;
			if (p.board[7][7].piece == 'w')
				adv_king_row_pieces++;
			/*if (p.board[1][0].piece == 'b')
				mid_row_pieces++;
			if (p.board[1][2].piece == 'b')
				mid_row_pieces++;
			if (p.board[1][4].piece == 'b')
				mid_row_pieces++;
			if (p.board[0][6].piece == 'b')
				mid_row_pieces++;*/
		}
		else
		{
			if (p.board[7][1].piece == 'w')
				king_row_pieces++;
			if (p.board[7][3].piece == 'w')
				king_row_pieces++;
			if (p.board[7][5].piece == 'w')
				king_row_pieces++;
			if (p.board[7][7].piece == 'w')
				king_row_pieces++;
			if (p.board[0][1].piece == 'b')
				adv_king_row_pieces++;
			if (p.board[0][3].piece == 'b')
				adv_king_row_pieces++;
			if (p.board[0][5].piece == 'b')
				adv_king_row_pieces++;
			if (p.board[0][7].piece == 'b')
				adv_king_row_pieces++;
		}
	}
		/*if (p.board[1][0].piece == 'w')
			mid_row_pieces++;
		if (p.board[1][2].piece == 'w')
			mid_row_pieces++;
		if (p.board[1][4].piece == 'w')
			mid_row_pieces++;
		if (p.board[0][6].piece == 'w')
			mid_row_pieces++;
	}*/
	if (p.turn < 20)
	{
		for (int i = 3; i < 5; i++)
		{
			for (int j = 1; j < 7; j++)
			{
				if (p.board[i][j].piece == p.color)
					center_pieces++;
				else if (p.board[i][j].piece = p.adv_color)
					adv_center_pieces++;
			}
		}
	}

	//number of checkers with empty spaces behind them, always back up pieces
	//whether or not you captured or sacrificed:
	//            a regular piece
	//            a King
	//think about setting it up to trade two-for-ones and three-for-twos
	//block in player's pieces

	//if (isMax)
	//if (p.piece_count + p.king_count < 7)
		//return count + k_count - adv_king_row_pieces * .2;
	//else
		
	/*if ((count > 8 && k_count > -1) || (count > -1 && k_count > 14))
	{
		threatened_pieces = 0;
		count = (p.piece_count - p.adv_count * 3) * 3;
		k_count = (p.king_count - p.adv_king_count * 3) * 5;
	}*/
	return count + k_count + (king_row_pieces - adv_king_row_pieces) * .15 - (threatened_pieces) * .3;// +(center_pieces - adv_center_pieces) * .1;// 
}

void Debug_Minimax(Player p, int l, string turn, bool isMax) {
	if (l == 0)
		cout << "///////////////////////////////////////////\n";
	cout << turn << " at depth level: " << l << '\n';
	//cout << p.move << " eval:" << h_val(p) << '\n';
	for (vector<Checker> vc : p.board)
	{
		for (Checker c : vc)
			cout << c.piece;
		cout << '\n';
	}
	if(l==0)
		cout << "///////////////////////////////////////////\n";
}
tuple<double, string> alphabeta(Player node, int depth, double alpha, double beta, bool maximizingPlayer) {
	
	//bool* isEmpty = new bool;
	//*isEmpty = false;

	if (depth == node.limit)
		return make_tuple(h_val(node, maximizingPlayer), node.move);

	//vector<Player> moves = actions(node, maximizingPlayer);	
	if (terminal_test(node))//, moves))
		return make_tuple(h_val(node, maximizingPlayer), node.move);

	tuple<double, string> v;

	if (maximizingPlayer) {
		v = make_tuple(-100000, "");
		for (Player p : actions(node, maximizingPlayer)) {
			//Debug_Minimax(p, depth, "Max", true);
			tuple<double, string>temp = alphabeta(p, depth + 1, alpha, beta, false);
			get<1>(temp) = p.move;
			if (get<0>(v) < get<0>(temp))
				v = temp;

			alpha = max(alpha, get<0>(v)); 	
			
			if (beta <= alpha)
				break;// return 1;//break (*beta cutoff*)		
		}			
		
		return v;
	}

	else {
		v = make_tuple(100000, "");
		for (Player p : actions(node, maximizingPlayer)) {
			//Debug_Minimax(p, depth, "Min", false);
			tuple<double, string> temp = alphabeta(p, depth + 1, alpha, beta, true); 	
			if (get<0>(v) > get<0>(temp))
				v = temp;

			beta = min(beta, get<0>(v));
			
			if (beta <= alpha) 
				break;			
		}

		return v;
	}
}

int main() {

	ofstream myfile;
	myfile.open("output.txt");
	Player p = Get_Player();		

	if (p.time <= 3 || p.type == 0) {
		myfile << actions(p, true)[0].move;
		return 0;
	}

	if (p.type == 1)
	{
		ifstream playdata("playdata.txt");
		string line;
		getline(playdata, line);
		playdata.close();		
		if (line.empty()) {			
			if (p.color == 'b')
			{
				ofstream playdata("playdata.txt");
				myfile << "E f6 e5";
				playdata << "1";
			}
			else
			{
				ofstream playdata("playdata.txt");
				myfile << "E e3 f4";
				playdata << "1";
			}			
			myfile.close();
			return 0;
		}
		else
			p.turn = stoi(line)+1;
	}
	
	if (p.time <= 5)
		p.limit = 1;
	else if (p.time <= 20) {
		p.limit = 3;
	}
	else if (p.time < 60)
	{
		p.limit = 5;
	}
	else if (p.time <= 100) {
		p.limit = 6;
	}
	else if (p.turn < 7)
		p.limit = 5;
	else if (p.turn < 11)
		p.limit = 6;
	else
		p.limit = 7;

	ifstream calibrate("calibrate.txt");
	string line;
	getline(calibrate, line);
	int c = stoi(line);
	if (c > 40000)
		p.limit--;

	ofstream playdata1("playdata.txt");

	int i = 1;
	int max_score = -1000;
	Player next_turn;
	vector<Player> next_moves = actions(p, true);
	
	if (next_moves.empty())
	{
		return 0;
	}
	if (next_moves.size() < 2)
	{
		myfile << next_moves[0].move;
		playdata1 << std::to_string(p.turn);
		return 0;
	}
	else 
	{	
		tuple<double, string> max = alphabeta(p, 0, -100000, 100000, true);
		myfile << get<1>(max);
	}

	playdata1 << std::to_string(p.turn);
	return 0;
}
