
#include "Parser.h"
#include "Statement.h"
#include <iostream>

using namespace std;


/* TODOS

Multiline string
Other types of number

*/

int is_digit_0(string* source, int index) {
	if (source->at(index) >= '0' && source->at(index) <= '9')
		return index + 1;
	return -1;
}
int is_digit_1(string* source, int index) {
	if (source->at(index) >= '1' && source->at(index) <= '9')
		return index + 1;
	return -1;
}
int is_alphabet_a(string* source, int index) {
	if (source->at(index) >= 'a' && source->at(index) <= 'z')
		return index + 1;
	return -1;
}
int is_alphabet_A(string* source, int index) {
	if (source->at(index) >= 'A' && source->at(index) <= 'Z')
		return index + 1;
	return -1;
}

int is_string_sl_dq(string* source, int index) {
	if (source->at(index) == '\n' || source->at(index) == '\"') {
		return -1;
	}
	return index + 1;
}

int is_comment(string* source, int index) {
	if (index >= source->size() || source->at(index) == '\n') {
		return -1;
	}
	return index + 1;
}

//Format B(REP, { Format(" ") });
//Format B(REP, { Format(OR, { Format(" "), Format(AND, {Format("\\"), Format(REP, {Format(" ")}), Format("\n")}) }) });
Format B(REP, { Format(" ") });
Format N(REP, { Format("\n") });
Format BN(REP, { Format(OR, {Format(" "), Format("\n")}) });

Format F_INT(OR, {
	Format("0"),
	Format(AND, {
		Format(FTN, is_digit_1),
		Format(REP, {Format(FTN, is_digit_0)})
	})
	});
Format F_VAR(AND, {
	Format(OR, {
		Format(FTN, is_alphabet_a),
		Format(FTN, is_alphabet_A),
		Format("_")
	}),
	Format(REP,{
		Format(OR,{
			Format(FTN, is_alphabet_a),
			Format(FTN, is_alphabet_A),
			Format(FTN, is_digit_0),
			Format("_")
		})
	})
	});
Format F_FLO(AND, {
	Format(OR,{
		Format("0."),
		Format(AND, {Format(FTN, is_digit_1), Format(REP, {Format(FTN, is_digit_0)}), Format(".")}),
		Format(".")
	}),
	Format(REP, {Format(FTN, is_digit_0)})
	});


S SB("ignore", B);
S SN("", AND, {
	S("", OR, {S("comment", AND, {
		S("#"), S("text", Format(REP, {Format(FTN, is_comment)}))
	}), S("") }),
	S("ignore", "\n")
});
S SBN("", REP, { S("", OR, {S(" "), SN}) });
// S SBN("ignore", BN);

S s_var("variable", AND, { S("address", F_VAR) });
S s_num("variable", OR, {
	S("int", F_INT),
	S("float", F_FLO)
}); // TODO: Expand to other numbers; float, 0xblah ...
S s_list("variable", OR, {
	S("list", AND, {
		S("["), SBN, S("variable", addr_calc), SBN, S("", REP,{
			S("", AND, {
				S(","), SBN, S("variable", addr_calc), SBN
			})
		}),
		S("]")
	}),
	S("list", AND, {
		S("["), SBN, S("variable", addr_calc), SBN, 
		S("listcomp", AND, { S("for"), SB, S("left", addr_tuple_nocalc), SB, S("in"), SB, S("right", addr_tuple), SBN }),
		S("]")
	}),
	S("list", AND, {
		S("["), SBN,
		S("]")
	})
});
S s_tuple("variable", AND, {
	S("tuple", AND, {
		S("("), SBN, S("variable", addr_calc), SBN, S("", REP,{
			S("", AND, {
				S(","), SBN, S("variable", addr_calc), SBN
			})
		}, 1),
		S(")")
	})
});
S s_string("variable", AND, {
	S("\""),
	S("string", Format(REP, {Format(FTN, is_string_sl_dq)})),
	S("\"")
});
S s_bracket("", AND, {
	S("("), SBN,
	S("", addr_calc),
	SBN, S(")")
	});

S s_attr("attribute", AND, {
	S(".", "."),
	S("name", F_VAR)
	});
S s_call("", OR, {
	S("calling", AND, {
		S("(", "("), SBN, 
		S("", OR, {
			S("parameter", AND, {SBN, S("name", F_VAR), SBN, S("="), SBN, S("value", addr_calc)}),
			S("parameter", AND, {SBN, S("value", addr_calc)})
		}),
		S("", REP, {
			S("", AND, {SBN, S(",", ","), SBN,
			S("", OR, {
					S("parameter", AND, {SBN, S("name", F_VAR), SBN, S("="), SBN, S("value", addr_calc)}),
					S("parameter", AND, {SBN, S("value", addr_calc)})
				})
			})
		}),
		SBN, S(")", ")")
	}),
	S("calling", AND, {
		S("(", "("), SBN,
		S(")", ")")
	})
});
S s_index("indexing", AND, {
	S("[", "["),
	S("variable", addr_calc), S("", REP, {
		S("", AND, {SBN, S(",", ","), SBN, S("variable", addr_calc)}),
	}), SBN,
	S("]", "]")
	});

vector<string> reserved{
"and", "assert", "break", "class", "continue", "def", "del", "elif", "else", "except", "exec", "finally",
"for", "from", "global", "if", "import", "in", "is", "lambda", "not", "or", "pass", "raise", "return",
"try", "while", "with", "yield"
};

Node single_addr(string* source, int* index) {
	int ind;
	Node target;
	// From variable list
	ind = *index;
	target = s_var.fit(source, &ind);
	if (ind != -1) {
		bool n = false;
		for (int i = 0; i < reserved.size(); i++) {
			if (reserved[i] == target.subnode[0].data) {
				n = true;
				break;
			}
		}
		if (!n) {
			*index = ind;
			return target;
		}
	}
	// Number
	ind = *index;
	target = s_num.fit(source, &ind);
	if (ind != -1) {
		*index = ind;
		return target;
	}
	// List
	ind = *index;
	target = s_list.fit(source, &ind);
	if (ind != -1) {
		*index = ind;
		return target;
	}
	// Tuple(with brackets)
	ind = *index;
	target = s_tuple.fit(source, &ind);
	if (ind != -1) {
		*index = ind;
		return target;
	}
	// String
	ind = *index;
	target = s_string.fit(source, &ind);
	if (ind != -1) {
		*index = ind;
		return target;
	}
	// Brackets
	ind = *index;
	target = s_bracket.fit(source, &ind);
	if (ind != -1) {
		*index = ind;
		return target;
	}
	*index = -1;
	return Node();
}

Node addr(string* source, int* index) {
	Node target = single_addr(source, index);
	if (*index == -1) {
		return Node();
	}

	while (true) {
		Node r;
		int ind;
		// attribute
		ind = *index;
		r = s_attr.fit(source, &ind);
		if (ind != -1) {
			*index = ind;
			r.subnode.insert(r.subnode.begin(), target);
			target = r;
			continue;
		}
		// call
		ind = *index;
		r = s_call.fit(source, &ind);
		if (ind != -1) {
			*index = ind;
			r.subnode.insert(r.subnode.begin(), target);
			target = r;
			continue;
		}
		// indexing
		ind = *index;
		r = s_index.fit(source, &ind);
		if (ind != -1) {
			*index = ind;
			r.subnode.insert(r.subnode.begin(), target);
			target = r;
			continue;
		}

		break;
	}
	return target;
}

// TODO: Add more at sings
S signs("", OR, {
	S("Tcalc_pow", AND, {SB, S("**"), SBN}),
	S("Tcalc_mod", AND, {SB, S("%"), SBN}),
	S("Tcalc_add", AND, {SB, S("+"), SBN}),
	S("Tcalc_substract", AND, {SB, S("-"), SBN}),
	S("Tcalc_multiply", AND, {SB, S("*"), SBN}),
	S("Tcalc_divide", AND, {SB, S("/"), SBN}),
	S("Tcalc_equal", AND, {SB, S("=="), SBN}),
	S("Tcalc_lt", AND, {SB, S("<"), SBN}),
	S("Tcalc_gt", AND, {SB, S(">"), SBN}),
	S("Tcalc_le", AND, {SB, S("<="), SBN}),
	S("Tcalc_ge", AND, {SB, S(">="), SBN}),
	S("Tcalc_nequal", AND, {SB, S("!="), SBN}),
	S("Tcalc_and", AND, {SB, S("and"), SBN}),
	S("Tcalc_or", AND, {SB, S("or"), SBN}),
	S("Tcalc_in", AND, {SB, S("in"), SBN}),
	S("Tcalc_nis", AND, {SB, S("is not"), SBN}),
	S("Tcalc_is", AND, {SB, S("is"), SBN}),
	S("Tcalc_nin", AND, {SB, S("not in"), SBN}),
});
S fsigns("", OR, {
	S("Tcalc_pos", AND, {
		SB, S("+"), SBN,
	}),
	S("Tcalc_unpack", AND, {
		SB, S("*"), SBN,
	}),
	S("Tcalc_neg", AND, {
		SB, S("-"), SBN,
	}),
	S("Tcalc_bneg", AND, {
		SB, S("~"), SBN,
	}),
	S("Tcalc_not", AND, {
		SB, S("not"), SBN,
	})
});

Node addr_calc(string* source, int* index) {

	vector<Node> calc_list;
	int ind = *index;
	Node target;
	while (true) {
		ind = *index;
		target = fsigns.fit(source, &ind);
		if (ind != -1) {
			calc_list.push_back(target);
			*index = ind;
			continue;
		}
		break;
	}

	target = addr(source, index);
	if (*index == -1) {
		return Node();
	}
	calc_list.push_back(target);

	while (true) {
		Node rs, rn;
		ind = *index;
		rs = signs.fit(source, &ind);

		vector<Node> temp;
		int iind = ind;
		while (true) {
			iind = ind;
			target = fsigns.fit(source, &iind);
			if (iind != -1) {
				temp.push_back(target);
				ind = iind;
				continue;
			}
			break;
		}

		rn = addr(source, &ind);
		if (ind != -1) {
			calc_list.push_back(rs);
			calc_list.insert(calc_list.end(), temp.begin(), temp.end());
			calc_list.push_back(rn);
			*index = ind;
			continue;
		}
		break;
	}

	// Reduce calc
	{
		for (int i = calc_list.size() - 1; i >= 0; i--) {
			if (calc_list[i].name == "Tcalc_pow") {
				if (i == 0 || i == calc_list.size() - 1) { *index = -1; break; }
				calc_list[i].subnode.insert(calc_list[i].subnode.begin(), { calc_list[i - 1] });
				calc_list[i].subnode.push_back(calc_list[i + 1]);
				calc_list[i].name.erase(calc_list[i].name.begin());
				calc_list.erase(calc_list.begin() + i + 1);
				calc_list.erase(calc_list.begin() + i - 1);
				i--;
				continue;
			}
			if (calc_list[i].name == "Tcalc_neg" || calc_list[i].name == "Tcalc_pos" || calc_list[i].name == "Tcalc_bneg" || calc_list[i].name == "Tcalc_unpack") {
				if (i == calc_list.size() - 1) { *index = -1; break; }
				calc_list[i].subnode.push_back(calc_list[i + 1]);
				calc_list[i].name.erase(calc_list[i].name.begin());
				calc_list.erase(calc_list.begin() + i + 1);
				continue;
			}
		}
		for (int i = 0; i < calc_list.size(); i++) {
			if (calc_list[i].name == "Tcalc_multiply" || calc_list[i].name == "Tcalc_divide" || calc_list[i].name == "Tcalc_mod") {
				if (i == 0 || i == calc_list.size() - 1) { *index = -1; break; }
				calc_list[i].subnode.insert(calc_list[i].subnode.begin(), { calc_list[i - 1] });
				calc_list[i].subnode.push_back(calc_list[i + 1]);
				calc_list[i].name.erase(calc_list[i].name.begin());
				calc_list.erase(calc_list.begin() + i + 1);
				calc_list.erase(calc_list.begin() + i - 1);
				i = 0;
				continue;
			}
		}
		for (int i = 0; i < calc_list.size(); i++) {
			if (calc_list[i].name == "Tcalc_add" || calc_list[i].name == "Tcalc_substract") {
				if (i == 0 || i == calc_list.size() - 1) { continue; }
				calc_list[i].subnode.insert(calc_list[i].subnode.begin(), { calc_list[i - 1] });
				calc_list[i].subnode.push_back(calc_list[i + 1]);
				calc_list[i].name.erase(calc_list[i].name.begin());
				calc_list.erase(calc_list.begin() + i + 1);
				calc_list.erase(calc_list.begin() + i - 1);
				i = 0;
				continue;
			}
		}
		for (int i = 0; i < calc_list.size(); i++) {
			if (calc_list[i].name == "Tcalc_equal" || calc_list[i].name == "Tcalc_nequal" ||
				calc_list[i].name == "Tcalc_in" || calc_list[i].name == "Tcalc_is" ||
				calc_list[i].name == "Tcalc_le" || calc_list[i].name == "Tcalc_lt" ||
				calc_list[i].name == "Tcalc_ge" || calc_list[i].name == "Tcalc_gt" ||
				calc_list[i].name == "Tcalc_nin" || calc_list[i].name == "Tcalc_nis") {
				if (i == 0 || i == calc_list.size() - 1) { *index = -1; break; }
				calc_list[i].subnode.insert(calc_list[i].subnode.begin(), { calc_list[i - 1] });
				calc_list[i].subnode.push_back(calc_list[i + 1]);
				calc_list[i].name.erase(calc_list[i].name.begin());
				calc_list.erase(calc_list.begin() + i + 1);
				calc_list.erase(calc_list.begin() + i - 1);
				i = 0;
				continue;
			}
		}
		for (int i = calc_list.size() - 1; i >= 0; i--)  {
			if (calc_list[i].name == "Tcalc_not") {
				if (i == calc_list.size() - 1) { *index = -1; break; }
				calc_list[i].subnode.push_back(calc_list[i + 1]);
				calc_list[i].name.erase(calc_list[i].name.begin());
				calc_list.erase(calc_list.begin() + i + 1);
				continue;
			}
		}
		for (int i = 0; i < calc_list.size(); i++) {
			if (calc_list[i].name == "Tcalc_and") {
				if (i == 0 || i == calc_list.size() - 1) { *index = -1; break; }
				calc_list[i].subnode.insert(calc_list[i].subnode.begin(), { calc_list[i - 1] });
				calc_list[i].subnode.push_back(calc_list[i + 1]);
				calc_list[i].name.erase(calc_list[i].name.begin());
				calc_list.erase(calc_list.begin() + i + 1);
				calc_list.erase(calc_list.begin() + i - 1);
				i = 0;
				continue;
			}
		}
		for (int i = 0; i < calc_list.size(); i++) {
			if (calc_list[i].name == "Tcalc_or") {
				if (i == 0 || i == calc_list.size() - 1) { *index = -1; break; }
				calc_list[i].subnode.insert(calc_list[i].subnode.begin(), { calc_list[i - 1] });
				calc_list[i].subnode.push_back(calc_list[i + 1]);
				calc_list[i].name.erase(calc_list[i].name.begin());
				calc_list.erase(calc_list.begin() + i + 1);
				calc_list.erase(calc_list.begin() + i - 1);
				i = 0;
				continue;
			}
		}
	}
	if (calc_list.size() > 1) {
		cout << "calc result longer than 1" << endl;
	}

	if (calc_list.size() == 0) {
		*index = -1;
		return Node();
	}

	return calc_list[0];
}

S s_btuple("tuple", REP, {
	S("", AND, {SB, S(","), SB, S("", addr_calc)})
	});

Node addr_tuple(string* source, int* index) {
	int ind = *index;
	Node t = addr_calc(source, &ind);
	if (ind == -1) {
		*index = -1;
		return Node();
	}
	*index = ind;
	Node tp = s_btuple.fit(source, &ind);
	if (ind == -1 || ind == *index) {
		return t;
	}
	tp.subnode.insert(tp.subnode.begin(), t);
	*index = ind;
	return tp;
}

Node addr_tuple_nocalc(string* source, int* index) {
	int ind = *index;
	Node t = addr(source, &ind);
	if (ind == -1) {
		*index = -1;
		return Node();
	}
	*index = ind;
	Node tp = s_btuple.fit(source, &ind);
	if (ind == -1 || ind == *index) {
		return t;
	}
	tp.subnode.insert(tp.subnode.begin(), t);
	*index = ind;
	return tp;
}


Format import_path(AND, {
	F_VAR, Format(REP, {Format(AND, {Format("."), F_VAR})})
	});

S phrase("", OR, {
	S("assign", AND, { S("left", addr_tuple_nocalc), SB, S("="), SBN, S("right", addr_tuple)}),
	S("assign_add", AND, { S("left", addr), SB, S("+="), SBN, S("right", addr_tuple)}),
	S("assign_sub", AND, { S("left", addr), SB, S("-="), SBN, S("right", addr_tuple)}),
	S("import", AND, {S("", OR, {S("from", AND, {S("from"), SB, S("path_from", import_path), SB}), S("")}),
		S("import"), S("ignore", Format(AND, {Format(" "), Format(REP, {Format(" ")})})),
		S("", OR, {
		S("", AND, {S("path", import_path), S("", OR, {S("", AND, {SB, S("as"), SB, S("name", F_VAR)}), S("")}),
		S("", REP, {S("", AND, {SB, S(","), SB, S("path", import_path), S("", OR, {S("", AND, {SB, S("as"), SB, S("name", F_VAR)}), S("")})})})
			}),
			S("*")
			}) }),
	S("return", AND, {S("return"), S("ignore", " "), SB, S("", addr_tuple)}),
	S("yield", AND, {S("yield"), S("ignore", " "), SB, S("", addr_tuple)}),
	S("evaluate", addr_tuple),
	S("break"),
	S("pass"),
	S("continue"),
	//S("comment", AND, {SB, S("#"), S("text", Format(REP, {Format(FTN, is_comment)}))}),
	});

// S rmv("ignore", Format(REP, { Format(AND, { Format(REP, {Format(" ")}), Format("\n") }) }));
// S rmv1("ignore", Format(AND, { Format(REP, {Format(" ")}), Format("\n") }));
S rmv("ignore", REP, { S("", AND, { SB, SN }) });
S rmv1("ignore", AND, { SB, SN });
// include # comment to rmv

S s_statement("", OR, {
	S("s_if", AND, {S("if"), SB, S("condition", addr_calc), SB, S(":"), SB, }), // TODO: add if statement without newline
	S("s_while", AND, {S("while"), SB, S("condition", addr_calc), SB, S(":"), SB}),
	S("s_for", AND, {S("for"), SB, S("left", addr_tuple_nocalc), SB, S("in"), SB, S("right", addr_tuple), SB, S(":"), SB}),
	S("s_def", AND, {S("def"), SB, S("name", F_VAR), SB, S("("), SB,
		S("", OR, {
		S("parameter", AND, { S("address", F_VAR), S("", OR, {S("default", AND, {SBN, S("="), SBN, S("", addr_calc)}), S("")}), SBN, S("", REP, {S("", AND, {S(","), SBN, S("address", F_VAR), S("", OR, {S("default", AND, {SBN, S("="), SBN, S("", addr_calc)}), S("")}), SBN})})
		}), SBN}), S(")"), SB, S(":"), SB}),
	S("s_class", AND, { S("class"), SB, S("name", F_VAR), SB, S("", OR, {S("", AND, {S("("), SBN, S("parent", import_path), S("", REP, {
		S("", AND, { S(","), SBN, S("parent", import_path), SBN })
	}), S(")")}), S("")}), SB, S(":"), SB }),
	S("s_elif", AND, {
		S("elif"), SB, S("condition", addr_calc), SB, S(":"), SB
	}),
	S("s_else", AND, {
		S("else"), SB, S(":"), SB
	}),
});

S s_comment("comment", AND, { S("#"), S("text", Format(REP, { Format(FTN, is_comment) })) });

Node statement(string* source, int* index, int indent) {
	vector<Node> ret;

	string indstr;
	for (int i = 0; i < indent; i++) {
		indstr.append(indent_text);
	}
	S s_indent("indent", indstr);

	S t_st("", AND, {
		s_indent,
		s_statement,
		SB,
		S("", OR, { s_comment, S("") }),
		S("\n"), 
	});
	S t_ph("", AND, {
		s_indent,
		phrase,
		SB,
		S("", OR, { s_comment, S("") })
	});
	S nl("", AND, { S("ignore", Format(REP, { Format(" ") })), S("\n") });
	S bl("", AND, {
		SB, S("", OR, { s_comment, S("")}), S("\n")
	});
	S blr("", AND, {
		SB, S("", OR, { s_comment, S("")})
	});

	int ind = *index;
	Node r;
	vector<Node> temp;
	int lastst = 0;
	for (int i = 0; true; i++) {
		temp.clear();
		ind = *index;
		if (i != 0) {
			r = nl.fit(source, &ind);
			temp.push_back(r);
			if (ind == -1) {
				return Node{ "", "", ret };
			}
		}

		int iind;

		iind = ind;
		r = t_st.fit(source, &iind);
		if (iind != -1) {
			*index = iind;
			temp.insert(temp.end(), r.subnode.begin(), r.subnode.end());

			// Lines in Statement
			int iiind = iind;
			Node lines = statement(source, &iind, indent + 1);

			if (iind == -1) {
				*index = iiind;
				return Node{ "", "", ret };
			}

			lines.name = "lines";

			temp.push_back(lines);
			lastst = temp.size() - 4;

			temp[lastst].subnode.insert(temp[lastst].subnode.end(), temp.begin() + lastst + 1, temp.end());
			temp.erase(temp.begin() + lastst + 1, temp.end());
			

			*index = iind;
			ret.insert(ret.end(), temp.begin(), temp.end());
			continue;
		}

		iind = ind;
		r = t_ph.fit(source, &iind);
		if (iind != -1) {
			temp.insert(temp.end(), r.subnode.begin(), r.subnode.end());
			*index = iind;
			ret.insert(ret.end(), temp.begin(), temp.end());
			continue;
		}

		iind = ind;
		r = bl.fit(source, &iind);
		if (iind != -1) {
			iind = ind;
			r = blr.fit(source, &iind);
			temp.insert(temp.end(), r.subnode.begin(), r.subnode.end());
			*index = iind;
			ret.insert(ret.end(), temp.begin(), temp.end());
			continue;
		}

		return Node{ "lines", "", ret };

	}


	/*
	int ind = *index;
	Node r;
	int iind;
	for (int i = 0; true; i++) {

		vector<Node> tn;
		if (i != 0) {
			ind = *index;
			r = SN.fit(source, &ind);
			if (ind == -1) {
				return Node{ "", "", ret };
			}
			tn.push_back(r);
		}

			iind = ind;
			r = S("ignore", Format(REP, {Format(" ")})).fit(source, &iind);
			if (iind != -1) {
				tn.push_back(r);
				ind = iind;
			}

			iind = ind;
			r = s_indent.fit(source, &ind);
			if (ind == -1) {
				return Node{ "", "", ret };
			}
			iind = ind;

			ind = iind;
			// Statement
			r = s_statement.fit(source, &ind);
			if (ind != -1) {
				tn.push_back(r);

				iind = ind;
				r = SB.fit(source, &iind);
				if (iind != -1) {
					tn.push_back(r);
					ind = iind;
				}

				int iind = ind;
				r = s_comment.fit(source, &iind);
				if (iind != -1) {
					tn.push_back(r);
					ind = iind;
				}
				ret.insert(ret.end(), tn.begin(), tn.end());
				tn.clear();
				*index = ind;

				continue;
			}

			ind = iind;
			// Phrase
			r = phrase.fit(source, &ind);
			if (ind != -1) {
				tn.push_back(r);

				iind = ind;
				r = SB.fit(source, &iind);
				if (iind != -1) {
					tn.push_back(r);
					ind = iind;
				}

				int iind = ind;
				r = s_comment.fit(source, &iind);
				if (iind != -1) {
					tn.push_back(r);
					ind = iind;
				}
				ret.insert(ret.end(), tn.begin(), tn.end());
				tn.clear();
				*index = ind;
				continue;
			}

			return Node{ "", "", ret };



	}
	*/
	/*
	int lastst = -1;

	int ind;
	for (int i = 0; true; i++) {
		ind = *index;
		if (i != 0) {
			Node r = rmv1.fit(source, &ind);
			Node r1 = rmv.fit(source, &ind);
			Node tind = s_indent.fit(source, &ind);
			if (ind == -1) {
				ind = *index;
				r1 = rmv.fit(source, &ind);
				if (ind != -1) {
					ret.push_back(r1);
					*index = ind;
				}
				return Node{ "", "", ret };
			}
			ret.push_back(r);
			ret.push_back(r1);
			ret.push_back(tind);
			*index = ind;
		}
		else {

			Node r = rmv1.fit(source, &ind);
			if (ind == -1) {
				ind = *index;
			}
			else {
				ret.push_back(r);
				*index = ind;
			}
			Node tind = s_indent.fit(source, &ind);
			if (ind != -1) {
				ret.push_back(tind);
				*index = ind;
			}
			else {
				return Node{ "", "", ret };
			}
		}

		ind = *index;
		Node tline;
		tline = s_statement.fit(source, &ind);
		if (ind != -1) {
			Node lines = statement(source, &ind, indent + 1);
			if (ind == -1) {
				return Node{ "", "", ret };
			}
			lines.name = "lines";
			tline.subnode.push_back(lines);
			ret.push_back(tline);
			if (tline.name == "else" || tline.name == "elif") {
				if (lastst == -1) {
					return Node{ "", "", ret };
				}
				ret[lastst].subnode.insert(ret[lastst].subnode.end(), ret.begin() + lastst + 1, ret.end());
				ret.erase(ret.begin() + lastst + 1, ret.end());
			}
			else {
				lastst = ret.size() - 1;
			}
			*index = ind;
			continue;
		}

		ind = *index;
		tline = phrase.fit(source, &ind);
		if (ind != -1) {
			ret.push_back(tline);
			*index = ind;
			lastst = -1;
			continue;
		}
		return Node{ "", "", ret };
		
	}
	*/
	
}

Node parse_source(string* source, int* index) {
	return statement(source, index, 0);
}
