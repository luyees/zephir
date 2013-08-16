/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is include which follows the "include" declaration
** in the input file. */
#include <stdio.h>
// 43 "parser.lemon"


#include "json/json.h"

#include "string.h"
#include "parser.h"
#include "scanner.h"
#include "xx.h"

static json_object *xx_ret_literal(int type, xx_parser_token *T, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	switch (type) {
		case XX_T_IDENTIFIER:
			json_object_object_add(ret, "type", json_object_new_string("variable"));
			break;
		case XX_T_INTEGER:
			json_object_object_add(ret, "type", json_object_new_string("int"));
			break;
		case XX_T_DOUBLE:
			json_object_object_add(ret, "type", json_object_new_string("double"));
			break;
		case XX_T_NULL:
			json_object_object_add(ret, "type", json_object_new_string("null"));
			break;
		case XX_T_STRING:
			json_object_object_add(ret, "type", json_object_new_string("string"));
			break;
		default:
			if (type == XX_T_TRUE) {
				json_object_object_add(ret, "type", json_object_new_string("bool"));
				json_object_object_add(ret, "value", json_object_new_string("true"));
			} else {
				if (type == XX_T_FALSE) {
					json_object_object_add(ret, "type", json_object_new_string("bool"));
					json_object_object_add(ret, "value", json_object_new_string("false"));
				} else {
					fprintf(stderr, "literal??\n");
				}
			}
	}

	if (T) {
		json_object_object_add(ret, "value", json_object_new_string(T->token));
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_expr(char *type, json_object *left, json_object *right, json_object *extra, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string(type));

	if (left) {
		json_object_object_add(ret, "left", left);
	}
	if (right) {
		json_object_object_add(ret, "right", right);
	}
	if (extra) {
		json_object_object_add(ret, "extra", extra);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_array_item(json_object *key, json_object *value, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	if (key) {
		json_object_object_add(ret, "key", key);
	}
	json_object_object_add(ret, "value", value);

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_namespace(xx_parser_token *T, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("namespace"));
	json_object_object_add(ret, "name", json_object_new_string(T->token));

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_class(xx_parser_token *T, json_object *class_definition, int is_abstract, xx_parser_token *E, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("class"));
	json_object_object_add(ret, "name", json_object_new_string(T->token));
	json_object_object_add(ret, "abtract", json_object_new_int(is_abstract));

	if (E) {
		json_object_object_add(ret, "extends", json_object_new_string(E->token));
	}

	if (class_definition) {
		json_object_object_add(ret, "definition", class_definition);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_class_definition(json_object *properties, json_object *methods, json_object *constants, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	if (properties) {
		json_object_object_add(ret, "properties", properties);
	}
	if (methods) {
		json_object_object_add(ret, "methods", methods);
	}
	if (constants) {
		json_object_object_add(ret, "constants", constants);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_class_property(json_object *visibility, xx_parser_token *T, json_object *default_value, xx_parser_token *D, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "visibility", visibility);
	json_object_object_add(ret, "type", json_object_new_string("property"));
	json_object_object_add(ret, "name", json_object_new_string(T->token));

	if (default_value) {
		json_object_object_add(ret, "default", default_value);
	}

	if (D) {
		json_object_object_add(ret, "docblock", json_object_new_string(D->token));
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_class_const(xx_parser_token *T, json_object *default_value, xx_parser_token *D, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("const"));
	json_object_object_add(ret, "name", json_object_new_string(T->token));
	json_object_object_add(ret, "default", default_value);

	if (D) {
		json_object_object_add(ret, "docblock", json_object_new_string(D->token));
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_class_method(json_object *visibility, xx_parser_token *T, json_object *parameters, json_object *statements, xx_parser_token *D, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "visibility", visibility);
	json_object_object_add(ret, "type", json_object_new_string("method"));
	json_object_object_add(ret, "name", json_object_new_string(T->token));

	if (parameters) {
		json_object_object_add(ret, "parameters", parameters);
	}

	if (statements) {
		json_object_object_add(ret, "statements", statements);
	}

	if (D) {
		json_object_object_add(ret, "docblock", json_object_new_string(D->token));
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_parameter(json_object *type, json_object *cast, xx_parser_token *N, json_object *default_value, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("parameter"));
	json_object_object_add(ret, "name", json_object_new_string(N->token));

	if (type) {
		json_object_object_add(ret, "data-type", type);
	}
	if (cast) {
		json_object_object_add(ret, "cast", cast);
	}
	if (default_value) {
		json_object_object_add(ret, "default", default_value);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_type(int type)
{
	switch (type) {
		case XX_TYPE_INTEGER:
			return json_object_new_string("int");
		case XX_TYPE_DOUBLE:
			return json_object_new_string("double");
		case XX_TYPE_BOOL:
			return json_object_new_string("bool");
		case XX_TYPE_STRING:
			return json_object_new_string("string");
		case XX_TYPE_ARRAY:
			return json_object_new_string("array");
		case XX_TYPE_VAR:
			return json_object_new_string("variable");
		default:
			fprintf(stderr, "err 1?\n");
	}

}

static json_object *xx_ret_list(json_object *list_left, json_object *right_list)
{
	json_object *ret;
	int i, array_length;

	ret = json_object_new_array();

	if (list_left) {

		if (json_object_get_type(list_left) == json_type_array) {
			array_length = json_object_array_length(list_left);
			for (i = 0; i < array_length; i++) {
				json_object_array_add(ret, json_object_array_get_idx(list_left, i));
			}
			//json_object_put(list_left);
		} else {
			json_object_array_add(ret, list_left);
		}

	}

	json_object_array_add(ret, right_list);

	return ret;
}

static json_object *xx_ret_let_statement(json_object *assignments, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("let"));
	json_object_object_add(ret, "assignments", assignments);

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_let_assignment(char *type, json_object *operator, xx_parser_token *V, xx_parser_token *P, json_object *index_expr, json_object *expr, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "assign-type", json_object_new_string(type));
	if (operator) {
		json_object_object_add(ret, "operator", operator);
	}
	json_object_object_add(ret, "variable", json_object_new_string(V->token));
	if (P) {
		json_object_object_add(ret, "property", json_object_new_string(P->token));
	}
	if (index_expr) {
		json_object_object_add(ret, "index-expr", index_expr);
	}
	if (expr) {
		json_object_object_add(ret, "expr", expr);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_if_statement(json_object *expr, json_object *statements, json_object *else_statements, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("if"));
	json_object_object_add(ret, "expr", expr);

	if (statements) {
		json_object_object_add(ret, "statements", statements);
	}

	if (else_statements) {
		json_object_object_add(ret, "else_statements", else_statements);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_switch_statement(json_object *expr, json_object *clauses, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("switch"));
	json_object_object_add(ret, "expr", expr);

	if (clauses) {
		json_object_object_add(ret, "clauses", clauses);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_case_clause(json_object *expr, json_object *statements, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("case"));

	if (expr) {
		json_object_object_add(ret, "expr", expr);
	}

	if (statements) {
		json_object_object_add(ret, "statements", statements);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_while_statement(json_object *expr, json_object *statements, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("while"));
	json_object_object_add(ret, "expr", expr);

	if (statements) {
		json_object_object_add(ret, "statements", statements);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_for_statement(json_object *expr, xx_parser_token *K, xx_parser_token *V, int reverse, json_object *statements, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("for"));
	json_object_object_add(ret, "expr", expr);

	if (K) {
		json_object_object_add(ret, "key", json_object_new_string(K->token));
	}
	if (V) {
		json_object_object_add(ret, "value", json_object_new_string(V->token));
	}

	json_object_object_add(ret, "reverse", json_object_new_int(reverse));

	if (statements) {
		json_object_object_add(ret, "statements", statements);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_loop_statement(json_object *statements, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("loop"));

	if (statements) {
		json_object_object_add(ret, "statements", statements);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_break_statement(xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("break"));

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_continue_statement(xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("continue"));

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_echo_statement(json_object *expressions, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("echo"));
	json_object_object_add(ret, "expressions", expressions);

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_return_statement(json_object *expr, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("return"));
	json_object_object_add(ret, "expr", expr);

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_mcall_statement(json_object *expr, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("mcall"));
	json_object_object_add(ret, "expr", expr);

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_unset_statement(json_object *domain, json_object *index, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("unset"));

	json_object_object_add(ret, "domain", domain);
	json_object_object_add(ret, "index", index);

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_declare_statement(int type, json_object *variables, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("declare"));

	switch (type) {
		case XX_T_TYPE_INTEGER:
			json_object_object_add(ret, "data-type", json_object_new_string("int"));
			break;
		case XX_T_TYPE_DOUBLE:
			json_object_object_add(ret, "data-type", json_object_new_string("double"));
			break;
		case XX_T_TYPE_BOOL:
			json_object_object_add(ret, "data-type", json_object_new_string("bool"));
			break;
		case XX_T_TYPE_STRING:
			json_object_object_add(ret, "data-type", json_object_new_string("string"));
			break;
		/*case XX_T_TYPE_ARRAY:
			json_object_object_add(ret, "data-type", json_object_new_string("array"));
			break;*/
		case XX_T_TYPE_VAR:
			json_object_object_add(ret, "data-type", json_object_new_string("variable"));
			break;
		default:
			fprintf(stderr, "err 2?\n");
	}

	json_object_object_add(ret, "variables", variables);

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_declare_variable(xx_parser_token *T, json_object *expr, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "variable", json_object_new_string(T->token));
	if (expr) {
		json_object_object_add(ret, "expr", expr);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_new_instance(xx_parser_token *T, json_object *parameters, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("new"));
	json_object_object_add(ret, "class", json_object_new_string(T->token));

	if (parameters) {
		json_object_object_add(ret, "parameters", parameters);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_throw_exception(xx_parser_token *T, json_object *parameters, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("throw"));
	json_object_object_add(ret, "domain", json_object_new_string(T->token));

	if (parameters) {
		json_object_object_add(ret, "parameters", parameters);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_fcall(xx_parser_token *F, json_object *parameters, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("fcall"));
	json_object_object_add(ret, "name", json_object_new_string(F->token));

	if (parameters) {
		json_object_object_add(ret, "parameters", parameters);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_mcall(xx_parser_token *O, xx_parser_token *M, json_object *parameters, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("mcall"));
	json_object_object_add(ret, "variable", json_object_new_string(O->token));
	json_object_object_add(ret, "name", json_object_new_string(M->token));

	if (parameters) {
		json_object_object_add(ret, "parameters", parameters);
	}

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}

static json_object *xx_ret_comment(xx_parser_token *T, xx_scanner_state *state)
{
	json_object *ret = json_object_new_object();

	json_object_object_add(ret, "type", json_object_new_string("comment"));
	json_object_object_add(ret, "value", json_object_new_string(T->token));

	json_object_object_add(ret, "file", json_object_new_string(state->active_file));
	json_object_object_add(ret, "line", json_object_new_int(state->active_line));
	json_object_object_add(ret, "char", json_object_new_int(state->active_char));

	return ret;
}


// 689 "parser.c"
/* Next is all token values, in a form suitable for use by makeheaders.
** This section will be null unless lemon is run with the -m switch.
*/
/* 
** These constants (all generated automatically by the parser generator)
** specify the various kinds of tokens (terminals) that the parser
** understands. 
**
** Each symbol here is a terminal symbol in the grammar.
*/
/* Make sure the INTERFACE macro is defined.
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/* The next thing included is series of defines which control
** various aspects of the generated parser.
**    YYCODETYPE         is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 terminals
**                       and nonterminals.  "int" is used otherwise.
**    YYNOCODE           is a number of type YYCODETYPE which corresponds
**                       to no legal terminal or nonterminal number.  This
**                       number is used to fill in empty slots of the hash 
**                       table.
**    YYFALLBACK         If defined, this indicates that one or more tokens
**                       have fall-back values which should be used if the
**                       original value of the token will not parse.
**    YYACTIONTYPE       is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 rules and
**                       states combined.  "int" is used otherwise.
**    xx_TOKENTYPE     is the data type used for minor tokens given 
**                       directly to the parser from the tokenizer.
**    YYMINORTYPE        is the data type used for all minor tokens.
**                       This is typically a union of many types, one of
**                       which is xx_TOKENTYPE.  The entry in the union
**                       for base tokens is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.
**    xx_ARG_SDECL     A static variable declaration for the %extra_argument
**    xx_ARG_PDECL     A parameter declaration for the %extra_argument
**    xx_ARG_STORE     Code to store %extra_argument into yypParser
**    xx_ARG_FETCH     Code to extract %extra_argument from yypParser
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
*/
#define YYCODETYPE unsigned char
#define YYNOCODE 142
#define YYACTIONTYPE unsigned short int
#define xx_TOKENTYPE xx_parser_token*
typedef union {
  xx_TOKENTYPE yy0;
  json_object* yy25;
  int yy283;
} YYMINORTYPE;
#define YYSTACKDEPTH 100
#define xx_ARG_SDECL xx_parser_status *status;
#define xx_ARG_PDECL ,xx_parser_status *status
#define xx_ARG_FETCH xx_parser_status *status = yypParser->status
#define xx_ARG_STORE yypParser->status = status
#define YYNSTATE 450
#define YYNRULE 223
#define YYERRORSYMBOL 80
#define YYERRSYMDT yy283
#define YY_NO_ACTION      (YYNSTATE+YYNRULE+2)
#define YY_ACCEPT_ACTION  (YYNSTATE+YYNRULE+1)
#define YY_ERROR_ACTION   (YYNSTATE+YYNRULE)

/* Next are that tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.  
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N < YYNSTATE                  Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   YYNSTATE <= N < YYNSTATE+YYNRULE   Reduce by rule N-YYNSTATE.
**
**   N == YYNSTATE+YYNRULE              A syntax error has occurred.
**
**   N == YYNSTATE+YYNRULE+1            The parser accepts its input.
**
**   N == YYNSTATE+YYNRULE+2            No such action.  Denotes unused
**                                      slots in the yy_action[] table.
**
** The action table is constructed as a single large table named yy_action[].
** Given state S and lookahead X, the action is computed as
**
**      yy_action[ yy_shift_ofst[S] + X ]
**
** If the index value yy_shift_ofst[S]+X is out of range or if the value
** yy_lookahead[yy_shift_ofst[S]+X] is not equal to X or if yy_shift_ofst[S]
** is equal to YY_SHIFT_USE_DFLT, it means that the action is not in the table
** and that yy_default[S] should be used instead.  
**
** The formula above is for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the yy_reduce_ofst[] array is used in place of
** the yy_shift_ofst[] array and YY_REDUCE_USE_DFLT is used in place of
** YY_SHIFT_USE_DFLT.
**
** The following are the tables generated in this section:
**
**  yy_action[]        A single table containing all actions.
**  yy_lookahead[]     A table containing the lookahead for each entry in
**                     yy_action.  Used to detect hash collisions.
**  yy_shift_ofst[]    For each state, the offset into yy_action for
**                     shifting terminals.
**  yy_reduce_ofst[]   For each state, the offset into yy_action for
**                     shifting non-terminals after a reduce.
**  yy_default[]       Default action for each state.
*/
static YYACTIONTYPE yy_action[] = {
 /*     0 */   286,  126,  128,  130,   83,  259,  206,  289,  257,   90,
 /*    10 */     3,    4,    5,    6,  265,  274,  280,  277,  143,  283,
 /*    20 */    44,   46,   48,  186,   57,   90,  187,  176,   68,   72,
 /*    30 */    77,   49,  188,   84,  309,   91,   92,   93,   94,   95,
 /*    40 */   258,  209,  187,  286,  229,  238,  241,  327,  371,  256,
 /*    50 */   289,   91,   92,   93,   94,   95,  263,  265,  274,  280,
 /*    60 */   277,  137,  283,   44,   46,   48,  451,   57,  159,  187,
 /*    70 */   168,   68,   72,   77,  178,  177,   84,  171,  172,  174,
 /*    80 */   173,  503,  167,   12,  372,  430,  286,  229,  238,  241,
 /*    90 */   310,    7,  256,  289,   10,  329,  333,  337,  436,  289,
 /*   100 */   265,  274,  280,  277,  217,  283,   44,   46,   48,  213,
 /*   110 */    57,  515,  187,  176,   68,   72,   77,  169,  170,   84,
 /*   120 */   171,  172,  174,  173,  160,  167,  179,  164,  175,  286,
 /*   130 */   229,  238,  241,   27,  107,  256,  289,  322,   63,  200,
 /*   140 */   350,  333,  337,  265,  274,  280,  277,  296,  283,   44,
 /*   150 */    46,   48,  255,   57,  266,  273,  207,   68,   72,   77,
 /*   160 */   178,  177,   84,  171,  172,  174,  173,  140,  193,  163,
 /*   170 */   164,  175,  286,  229,  238,  241,  315,    7,  256,  289,
 /*   180 */    10,  109,  111,  512,  436,  289,  265,  274,  280,  277,
 /*   190 */   217,  283,   44,   46,   48,  222,   57,  514,  187,   79,
 /*   200 */    68,   72,   77,  169,  170,   84,  171,  172,  174,  173,
 /*   210 */   110,  368,  370,  373,  377,  286,  229,  238,  241,   71,
 /*   220 */   361,  256,  289,  350,  333,  337,  275,  273,  268,  265,
 /*   230 */   274,  280,  277,  234,  283,   44,   46,   48,   98,   57,
 /*   240 */   233,  187,  212,   68,   72,   77,  187,  145,   84,  380,
 /*   250 */   267,  407,  408,  409,  410,  187,  278,  273,  286,  229,
 /*   260 */   238,  241,   52,  151,  256,  289,  162,  407,  408,  409,
 /*   270 */   410,  187,  265,  274,  280,  277,  401,  283,   44,   46,
 /*   280 */    48,  143,   57,   85,  228,  143,   68,   72,   77,  187,
 /*   290 */   201,   84,  417,  187,  184,  189,   18,   19,  530,  189,
 /*   300 */   367,  286,  229,  238,  241,  307,   98,  256,  289,  167,
 /*   310 */   208,   80,  281,  273,  187,  265,  274,  280,  277,  187,
 /*   320 */   283,   44,   46,   48,  143,   57,  284,  273,  161,   68,
 /*   330 */    72,   77,  187,  262,   84,  234,  374,  375,  189,  376,
 /*   340 */   529,  230,  235,  187,  286,  229,  238,  241,  358,   98,
 /*   350 */   256,  289,  247,  221,  381,  166,  382,  187,  265,  274,
 /*   360 */   280,  277,   62,  283,   44,   46,   48,    9,   57,   61,
 /*   370 */   314,  504,   68,   72,   77,   60,  400,   84,  411,  328,
 /*   380 */   439,  352,  443,   64,  367,  311,  528,  286,  229,  238,
 /*   390 */   241,  321,  194,  256,  289,  369,   64,  527,  311,  250,
 /*   400 */   253,  265,  274,  280,  277,  143,  283,   44,   46,   48,
 /*   410 */   526,   57,  323,  187,  190,   68,   72,   77,   98,  189,
 /*   420 */    84,   98,   97,  186,   73,  216,  187,  309,  502,  187,
 /*   430 */   286,  229,  238,  241,  360,  187,  256,  289,  186,  196,
 /*   440 */   198,   11,  268,  268,  265,  274,  280,  277,  378,  283,
 /*   450 */    44,   46,   48,   98,   57,  402,  185,  225,   68,   72,
 /*   460 */    77,  187,  403,   84,  276,  279,  108,   58,  525,  268,
 /*   470 */   309,  191,  254,  286,  229,  238,  241,   56,  187,  256,
 /*   480 */   289,  505,   87,  268,  379,  343,  524,  265,  274,  280,
 /*   490 */   277,  282,  283,   44,   46,   48,  328,   57,  389,  292,
 /*   500 */   299,   68,   72,   77,   86,  285,   84,  187,  187,  243,
 /*   510 */   246,  352,  503,  304,  367,  372,  286,  229,  238,  241,
 /*   520 */   320,  187,  256,  289,  186,  505,  204,  390,  379,  362,
 /*   530 */   265,  274,  280,  277,  187,  283,   44,   46,   48,  101,
 /*   540 */    57,  342,  113,  115,   68,   72,   77,  187,  117,   84,
 /*   550 */   187,  187,  211,  392,  106,  391,  187,  202,  339,  286,
 /*   560 */   229,  238,  241,  316,  119,  256,  289,  340,  394,  121,
 /*   570 */   248,  395,  187,  265,  274,  280,  277,  187,  283,   44,
 /*   580 */    46,   48,  123,   57,  418,  125,  127,   68,   72,   77,
 /*   590 */   187,  419,   84,  187,  187,  214,   59,  397,  232,  338,
 /*   600 */   104,  396,  286,  229,  238,  241,  364,  129,  256,  289,
 /*   610 */   336,  215,  335,  105,  334,  187,  265,  274,  280,  277,
 /*   620 */   231,  283,   44,   46,   48,  131,   57,  406,  133,  239,
 /*   630 */    68,   72,   77,  187,  332,   84,  187,  187,  252,  331,
 /*   640 */   182,  181,  324,  404,  405,  286,  229,  238,  241,  302,
 /*   650 */   135,  256,  289,  444,  659,  220,  658,  318,  187,  265,
 /*   660 */   274,  280,  277,   14,  283,   44,   46,   48,  657,   57,
 /*   670 */   317,   24,  312,   68,   72,   77,  223,   54,   84,  224,
 /*   680 */    53,   50,  502,  445,  142,   96,  504,  298,  286,  229,
 /*   690 */   238,  241,  366,  244,  256,  289,  297,  165,  141,  251,
 /*   700 */    88,  420,  265,  274,  280,  277,   69,  283,   44,   46,
 /*   710 */    48,  421,   57,   89,  288,  447,   68,   72,   77,   74,
 /*   720 */   287,   84,  270,   22,   47,   21,  450,  271,  272,  269,
 /*   730 */    20,  286,  229,  238,  241,  295,  431,  256,  289,   45,
 /*   740 */   432,   78,  242,  149,  434,  265,  274,  280,  277,  264,
 /*   750 */   283,   44,   46,   48,  261,   57,  148,  437,  260,   68,
 /*   760 */    72,   77,  438,  501,   84,  237,  249,  245,  199,    8,
 /*   770 */   363,  507,  147,  197,  286,  229,  238,  241,   76,  357,
 /*   780 */   256,  289,  441,   65,  354,  509,  112,  195,  265,  274,
 /*   790 */   280,  277,  513,  283,   44,   46,   48,  506,   57,   66,
 /*   800 */   508,  510,   68,   72,   77,  355,  353,   84,  511,  366,
 /*   810 */   366,  366,  366,  366,  366,  366,  366,  286,  229,  238,
 /*   820 */   241,  325,  366,  256,  289,  366,  366,  366,  366,  366,
 /*   830 */   366,  265,  274,  280,  277,  366,  283,   44,   46,   48,
 /*   840 */   366,   57,  366,  366,  366,   68,   72,   77,  366,  366,
 /*   850 */    84,  366,  366,  366,  366,  366,  366,  366,  366,  366,
 /*   860 */   286,  229,  238,  241,  308,  366,  256,  289,  366,  366,
 /*   870 */   366,  366,  366,  366,  265,  274,  280,  277,  366,  283,
 /*   880 */    44,   46,   48,  366,   57,  366,  366,  366,   68,   72,
 /*   890 */    77,  366,  366,   84,  366,  366,  366,  366,  366,  366,
 /*   900 */   366,  366,  366,  286,  229,  238,  241,   25,  366,  256,
 /*   910 */   289,  366,  412,  413,  414,  415,  366,  265,  274,  280,
 /*   920 */   277,  366,  283,   44,   46,   48,  366,   57,  366,  366,
 /*   930 */   366,   68,   72,   77,  366,  366,   84,  366,  368,  370,
 /*   940 */   373,  377,  366,  366,  286,  366,  388,  229,  238,  241,
 /*   950 */   380,  289,  256,  366,  412,  413,  414,  415,  265,  274,
 /*   960 */   280,  277,  366,  283,   44,   46,   48,  468,   57,  366,
 /*   970 */   351,  366,   68,   72,   77,  366,  380,   84,   43,  366,
 /*   980 */   366,  366,  366,  366,  366,  366,   90,  366,  229,  238,
 /*   990 */   241,  319,  380,  256,  366,  366,  290,   29,   30,   31,
 /*  1000 */    32,   33,   34,   35,   36,   37,   38,   39,   40,   41,
 /*  1010 */    42,   43,   91,   92,   93,   94,   95,  205,  218,  366,
 /*  1020 */   226,  227,  236,  366,   26,  366,  366,  366,  366,  290,
 /*  1030 */    29,   30,   31,   32,   33,   34,   35,   36,   37,   38,
 /*  1040 */    39,   40,   41,   42,  366,  366,   43,  674,    1,    2,
 /*  1050 */   449,    4,    5,    6,  366,  236,  366,  366,  366,  294,
 /*  1060 */   366,  366,  366,  366,  290,   29,   30,   31,   32,   33,
 /*  1070 */    34,   35,   36,   37,   38,   39,   40,   41,   42,   43,
 /*  1080 */   383,   17,  398,  416,  366,  411,  422,  386,   19,  366,
 /*  1090 */   236,  367,  301,  366,  366,  366,  366,  290,   29,   30,
 /*  1100 */    31,   32,   33,   34,   35,   36,   37,   38,   39,   40,
 /*  1110 */    41,   42,   43,  366,  425,  398,  416,  366,  411,  366,
 /*  1120 */   386,   19,  366,  236,  367,  359,  412,  413,  414,  415,
 /*  1130 */   290,   29,   30,   31,   32,   33,   34,   35,   36,   37,
 /*  1140 */    38,   39,   40,   41,   42,  366,  366,   43,  366,  368,
 /*  1150 */   370,  373,  377,  366,  366,  471,  236,  366,  426,  366,
 /*  1160 */   313,  366,  366,  366,  380,  290,   29,   30,   31,   32,
 /*  1170 */    33,   34,   35,   36,   37,   38,   39,   40,   41,   42,
 /*  1180 */    43,  366,  366,  388,  366,  366,  366,  380,  366,  366,
 /*  1190 */   366,  236,  366,  306,  366,  366,  366,  366,  290,   29,
 /*  1200 */    30,   31,   32,   33,   34,   35,   36,   37,   38,   39,
 /*  1210 */    40,   41,   42,   43,  366,  384,  366,  366,  366,  366,
 /*  1220 */   385,  386,   19,  366,  236,  367,  365,  366,  366,  366,
 /*  1230 */   366,  290,   29,   30,   31,   32,   33,   34,   35,   36,
 /*  1240 */    37,   38,   39,   40,   41,   42,  366,  366,   43,  366,
 /*  1250 */   366,  366,  366,  366,  366,  366,  366,  236,  366,  366,
 /*  1260 */   366,   70,  366,  366,  366,  366,  290,   29,   30,   31,
 /*  1270 */    32,   33,   34,   35,   36,   37,   38,   39,   40,   41,
 /*  1280 */    42,   43,  366,  366,  366,  366,  366,  366,  366,  366,
 /*  1290 */   366,  366,  236,  366,   75,  366,  366,  366,  366,  290,
 /*  1300 */    29,   30,   31,   32,   33,   34,   35,   36,   37,   38,
 /*  1310 */    39,   40,   41,   42,   43,  366,  366,  366,  366,  366,
 /*  1320 */   366,  366,  366,  366,  366,  236,  366,   82,  366,  366,
 /*  1330 */   366,  366,  290,   29,   30,   31,   32,   33,   34,   35,
 /*  1340 */    36,   37,   38,   39,   40,   41,   42,  366,  366,   43,
 /*  1350 */   366,  366,  366,  366,  366,  366,  366,  366,  236,  366,
 /*  1360 */   366,  366,   55,  366,  366,  366,  366,  290,   29,   30,
 /*  1370 */    31,   32,   33,   34,   35,   36,   37,   38,   39,   40,
 /*  1380 */    41,   42,   43,  366,  366,  366,  366,  366,  366,  366,
 /*  1390 */   366,  366,  366,  236,  366,   67,  366,  366,  366,  366,
 /*  1400 */   290,   29,   30,   31,   32,   33,   34,   35,   36,   37,
 /*  1410 */    38,   39,   40,   41,   42,   43,  366,  366,  366,  366,
 /*  1420 */   366,  366,  366,  366,  366,  366,  236,  366,  326,  366,
 /*  1430 */   366,  366,  366,  290,   29,   30,   31,   32,   33,   34,
 /*  1440 */    35,   36,   37,   38,   39,   40,   41,   42,  366,  366,
 /*  1450 */    43,  366,  366,  366,  366,  366,  366,  366,  366,  236,
 /*  1460 */   366,  366,  366,   51,  366,  366,  366,  366,  290,   29,
 /*  1470 */    30,   31,   32,   33,   34,   35,   36,   37,   38,   39,
 /*  1480 */    40,   41,   42,   43,  366,  366,  366,  366,  366,  366,
 /*  1490 */   366,  366,  366,  366,  236,  366,  366,  366,  366,  366,
 /*  1500 */   366,   28,   29,   30,   31,   32,   33,   34,   35,   36,
 /*  1510 */    37,   38,   39,   40,   41,   42,  366,  366,  366,  366,
 /*  1520 */   366,  103,  146,  150,  144,  180,  100,  236,  139,  366,
 /*  1530 */   103,  146,  150,  144,  180,  100,  366,  139,  366,  136,
 /*  1540 */   366,  366,  366,  366,  366,  366,  366,  366,  136,  203,
 /*  1550 */   366,  366,  103,  146,  150,  144,  180,  100,  366,  139,
 /*  1560 */   366,  366,  366,  366,  366,  366,  366,  366,  158,  219,
 /*  1570 */   136,  183,  366,  366,  366,  152,  153,  158,  154,  155,
 /*  1580 */   156,  157,  366,  366,  152,  153,  366,  154,  155,  156,
 /*  1590 */   157,  366,  366,  366,  366,  366,  366,  366,  366,  158,
 /*  1600 */   366,  366,  366,  366,  366,  366,  152,  153,  366,  154,
 /*  1610 */   155,  156,  157,  366,  366,  366,  366,  366,  366,  103,
 /*  1620 */   146,  150,  144,  180,  100,  366,  139,  424,  366,  427,
 /*  1630 */   428,  416,  366,  411,  385,  386,   19,  136,  366,  367,
 /*  1640 */   366,  103,  146,  150,  144,  180,  100,  366,  139,  366,
 /*  1650 */   366,  366,  341,  366,  366,  366,  366,  366,  303,  136,
 /*  1660 */   192,  366,  366,  366,  366,  366,  158,  366,  330,  366,
 /*  1670 */   366,  366,  366,  152,  153,  366,  154,  155,  156,  157,
 /*  1680 */   356,  366,  344,  345,  346,  347,  348,  349,  158,  341,
 /*  1690 */   366,  366,  366,  366,  366,  152,  153,  366,  154,  155,
 /*  1700 */   156,  157,  366,  366,  366,  330,  366,  366,  103,  146,
 /*  1710 */   150,  144,  180,  100,  366,  139,  366,   23,  366,  344,
 /*  1720 */   345,  346,  347,  348,  349,  366,  136,  366,  366,  366,
 /*  1730 */   103,  146,  150,  144,  180,  100,  366,  139,  366,  366,
 /*  1740 */   366,  341,  366,  366,  366,  366,  366,  366,  136,  366,
 /*  1750 */   366,  366,  366,  366,  366,  158,  210,  330,  366,  366,
 /*  1760 */   366,  366,  152,  153,  366,  154,  155,  156,  157,  291,
 /*  1770 */   366,  344,  345,  346,  347,  348,  349,  158,  412,  413,
 /*  1780 */   414,  415,  366,  366,  152,  153,  366,  154,  155,  156,
 /*  1790 */   157,  366,  366,  366,  366,  366,  366,  103,  146,  150,
 /*  1800 */   144,  180,  100,  366,  139,  366,  366,   15,  366,  366,
 /*  1810 */   399,  366,  393,  366,  366,  136,  380,  366,  366,  366,
 /*  1820 */   366,  366,  132,  134,   99,  114,  118,  120,  122,  124,
 /*  1830 */   116,  102,  126,  128,  130,  366,  366,  366,  366,  366,
 /*  1840 */   366,  366,  366,  366,  158,  366,  366,  366,  366,  366,
 /*  1850 */   366,  152,  153,  366,  154,  155,  156,  157,  132,  134,
 /*  1860 */    99,  114,  118,  120,  122,  124,  116,  102,  126,  128,
 /*  1870 */   130,   99,  114,  118,  120,  122,  124,  116,  102,  126,
 /*  1880 */   128,  130,  366,  587,  366,  412,  413,  414,  415,  366,
 /*  1890 */   138,  366,  366,  366,  366,  366,  366,  132,  134,   99,
 /*  1900 */   114,  118,  120,  122,  124,  116,  102,  126,  128,  130,
 /*  1910 */   366,  366,  366,  366,  435,  366,  366,  399,  366,  393,
 /*  1920 */   293,  366,  366,  380,  366,  366,  366,  132,  134,   99,
 /*  1930 */   114,  118,  120,  122,  124,  116,  102,  126,  128,  130,
 /*  1940 */   366,  412,  413,  414,  415,  368,  370,  373,  377,  366,
 /*  1950 */   300,  366,  366,  366,  366,  366,  366,  132,  134,   99,
 /*  1960 */   114,  118,  120,  122,  124,  116,  102,  126,  128,  130,
 /*  1970 */   442,  366,  366,  399,  474,  393,  366,  351,  366,  380,
 /*  1980 */   305,  366,  366,  380,  366,  366,  366,  132,  134,   99,
 /*  1990 */   114,  118,  120,  122,  124,  116,  102,  126,  128,  130,
 /*  2000 */   366,  366,  366,  366,  366,  366,  366,  366,  366,  366,
 /*  2010 */   671,  366,  366,  366,  366,  366,  366,  132,  134,   99,
 /*  2020 */   114,  118,  120,  122,  124,  116,  102,  126,  128,  130,
 /*  2030 */   366,  412,  413,  414,  415,  368,  370,  373,  377,  366,
 /*  2040 */    81,  366,  366,  366,  366,  366,  366,  132,  134,   99,
 /*  2050 */   114,  118,  120,  122,  124,  116,  102,  126,  128,  130,
 /*  2060 */   448,  366,  366,  399,  469,  393,  366,  351,  240,  380,
 /*  2070 */   366,  366,  366,  380,  366,  132,  134,   99,  114,  118,
 /*  2080 */   120,  122,  124,  116,  102,  126,  128,  130,   13,   16,
 /*  2090 */   423,  429,  428,  416,  366,  411,  422,  386,   19,  366,
 /*  2100 */   366,  367,  366,  366,  366,  366,  446,   16,  423,  429,
 /*  2110 */   428,  416,  366,  411,  422,  386,   19,  366,  366,  367,
 /*  2120 */   366,  368,  370,  373,  377,  368,  370,  373,  377,  366,
 /*  2130 */   366,  366,  366,  366,  366,  366,  433,   16,  423,  429,
 /*  2140 */   428,  416,  366,  411,  422,  386,   19,  366,  366,  367,
 /*  2150 */   470,  366,  366,  387,  473,  393,  366,  351,  366,  380,
 /*  2160 */   366,  366,  366,  380,  440,   16,  423,  429,  428,  416,
 /*  2170 */   366,  411,  422,  386,   19,  366,  366,  367,  366,  412,
 /*  2180 */   413,  414,  415,  366,  366,  366,  366,  366,  366,  366,
 /*  2190 */   366,  366,  366,  366,  366,  366,  366,  366,  366,  366,
 /*  2200 */   366,  366,  366,  366,  366,  366,  366,  366,  467,  366,
 /*  2210 */   366,  399,  366,  393,  366,  366,  366,  380,  366,  412,
 /*  2220 */   413,  414,  415,  368,  370,  373,  377,  366,  366,  366,
 /*  2230 */   366,  366,  366,  366,  366,  366,  366,  366,  366,  366,
 /*  2240 */   366,  366,  366,  366,  366,  366,  366,  366,  466,  366,
 /*  2250 */   366,  399,  472,  393,  366,  351,  366,  380,  366,  366,
 /*  2260 */   366,  380,
};
static YYCODETYPE yy_lookahead[] = {
 /*     0 */    26,   16,   17,   18,   30,   23,   26,   33,   26,   34,
 /*    10 */    84,   85,   86,   87,   40,   41,   42,   43,  123,   45,
 /*    20 */    46,   47,   48,    5,   50,   34,  131,   26,   54,   55,
 /*    30 */    56,  120,  137,   59,  123,   60,   61,   62,   63,   64,
 /*    40 */    27,   66,  131,   26,   70,   71,   72,   30,    3,   75,
 /*    50 */    33,   60,   61,   62,   63,   64,   38,   40,   41,   42,
 /*    60 */    43,  123,   45,   46,   47,   48,    0,   50,   67,  131,
 /*    70 */    26,   54,   55,   56,   73,   74,   59,   76,   77,   78,
 /*    80 */    79,   36,   94,   29,   39,   31,   26,   70,   71,   72,
 /*    90 */    30,   25,   75,   33,   28,  102,  103,  104,   32,   33,
 /*   100 */    40,   41,   42,   43,  123,   45,   46,   47,   48,  128,
 /*   110 */    50,   36,  131,   26,   54,   55,   56,   73,   74,   59,
 /*   120 */    76,   77,   78,   79,  136,   94,  138,  139,  140,   26,
 /*   130 */    70,   71,   72,   30,   26,   75,   33,   99,  122,   37,
 /*   140 */   102,  103,  104,   40,   41,   42,   43,    5,   45,   46,
 /*   150 */    47,   48,   27,   50,  133,  134,  126,   54,   55,   56,
 /*   160 */    73,   74,   59,   76,   77,   78,   79,   65,   66,  138,
 /*   170 */   139,  140,   26,   70,   71,   72,   30,   25,   75,   33,
 /*   180 */    28,   73,   74,   36,   32,   33,   40,   41,   42,   43,
 /*   190 */   123,   45,   46,   47,   48,  128,   50,   36,  131,   57,
 /*   200 */    54,   55,   56,   73,   74,   59,   76,   77,   78,   79,
 /*   210 */    67,    1,    2,    3,    4,   26,   70,   71,   72,   30,
 /*   220 */    99,   75,   33,  102,  103,  104,  133,  134,    5,   40,
 /*   230 */    41,   42,   43,  123,   45,   46,   47,   48,  123,   50,
 /*   240 */   130,  131,  127,   54,   55,   56,  131,  123,   59,   39,
 /*   250 */    27,    1,    2,    3,    4,  131,  133,  134,   26,   70,
 /*   260 */    71,   72,   30,  123,   75,   33,    5,    1,    2,    3,
 /*   270 */     4,  131,   40,   41,   42,   43,   26,   45,   46,   47,
 /*   280 */    48,  123,   50,  124,  125,  123,   54,   55,   56,  131,
 /*   290 */   132,   59,   26,  131,  132,  137,   97,   98,   26,  137,
 /*   300 */   101,   26,   70,   71,   72,   30,  123,   75,   33,   94,
 /*   310 */   127,  123,  133,  134,  131,   40,   41,   42,   43,  131,
 /*   320 */    45,   46,   47,   48,  123,   50,  133,  134,   67,   54,
 /*   330 */    55,   56,  131,  132,   59,  123,    1,    2,  137,    4,
 /*   340 */    26,  129,  130,  131,   26,   70,   71,   72,   30,  123,
 /*   350 */    75,   33,   26,  127,    2,  140,    4,  131,   40,   41,
 /*   360 */    42,   43,   30,   45,   46,   47,   48,   27,   50,  121,
 /*   370 */   122,   36,   54,   55,   56,   30,   93,   59,   95,    5,
 /*   380 */    29,   98,   31,   51,  101,   53,   26,   26,   70,   71,
 /*   390 */    72,   30,   26,   75,   33,    3,   51,   26,   53,   73,
 /*   400 */    74,   40,   41,   42,   43,  123,   45,   46,   47,   48,
 /*   410 */    26,   50,   38,  131,  132,   54,   55,   56,  123,  137,
 /*   420 */    59,  123,  127,    5,  120,  127,  131,  123,   36,  131,
 /*   430 */    26,   70,   71,   72,   30,  131,   75,   33,    5,   73,
 /*   440 */    74,   26,    5,    5,   40,   41,   42,   43,    3,   45,
 /*   450 */    46,   47,   48,  123,   50,   27,   38,  127,   54,   55,
 /*   460 */    56,  131,   34,   59,   27,   27,   67,  120,   26,    5,
 /*   470 */   123,   38,   67,   26,   70,   71,   72,   30,  131,   75,
 /*   480 */    33,   36,    5,    5,   39,   11,   26,   40,   41,   42,
 /*   490 */    43,   27,   45,   46,   47,   48,    5,   50,   26,  123,
 /*   500 */   123,   54,   55,   56,   27,   27,   59,  131,  131,   65,
 /*   510 */    66,   98,   36,  123,  101,   39,   26,   70,   71,   72,
 /*   520 */    30,  131,   75,   33,    5,   36,  123,   34,   39,   38,
 /*   530 */    40,   41,   42,   43,  131,   45,   46,   47,   48,  123,
 /*   540 */    50,   26,  123,  123,   54,   55,   56,  131,  123,   59,
 /*   550 */   131,  131,  126,   27,   66,   94,  131,   38,   34,   26,
 /*   560 */    70,   71,   72,   30,  123,   75,   33,   94,   26,  123,
 /*   570 */    67,   34,  131,   40,   41,   42,   43,  131,   45,   46,
 /*   580 */    47,   48,  123,   50,   27,  123,  123,   54,   55,   56,
 /*   590 */   131,   34,   59,  131,  131,   67,   29,   27,    5,   26,
 /*   600 */   135,   94,   26,   70,   71,   72,   30,  123,   75,   33,
 /*   610 */    94,  126,   34,   26,   26,  131,   40,   41,   42,   43,
 /*   620 */    27,   45,   46,   47,   48,  123,   50,   95,  123,  123,
 /*   630 */    54,   55,   56,  131,   94,   59,  131,  131,   27,   34,
 /*   640 */    37,   26,   29,   94,   27,   26,   70,   71,   72,   30,
 /*   650 */   123,   75,   33,   26,   52,  126,   52,   29,  131,   40,
 /*   660 */    41,   42,   43,   30,   45,   46,   47,   48,   52,   50,
 /*   670 */    49,   29,   52,   54,   55,   56,   67,   29,   59,  126,
 /*   680 */    49,   29,   36,   29,   37,  126,   36,   57,   26,   70,
 /*   690 */    71,   72,   30,   26,   75,   33,   26,   52,   26,   67,
 /*   700 */   125,   94,   40,   41,   42,   43,   29,   45,   46,   47,
 /*   710 */    48,   27,   50,   26,   26,   30,   54,   55,   56,   29,
 /*   720 */    65,   59,   26,   37,   27,   26,    0,   34,   94,  134,
 /*   730 */    36,   26,   70,   71,   72,   30,   26,   75,   33,   27,
 /*   740 */    29,   26,   26,  135,   30,   40,   41,   42,   43,   27,
 /*   750 */    45,   46,   47,   48,   37,   50,    5,   28,   26,   54,
 /*   760 */    55,   56,   26,   36,   59,   27,   27,   27,   67,   26,
 /*   770 */    29,   36,   26,   67,   26,   70,   71,   72,   30,   29,
 /*   780 */    75,   33,   30,   94,   26,   36,   67,   67,   40,   41,
 /*   790 */    42,   43,   36,   45,   46,   47,   48,   36,   50,   52,
 /*   800 */    36,   36,   54,   55,   56,   37,   36,   59,   36,  141,
 /*   810 */   141,  141,  141,  141,  141,  141,  141,   26,   70,   71,
 /*   820 */    72,   30,  141,   75,   33,  141,  141,  141,  141,  141,
 /*   830 */   141,   40,   41,   42,   43,  141,   45,   46,   47,   48,
 /*   840 */   141,   50,  141,  141,  141,   54,   55,   56,  141,  141,
 /*   850 */    59,  141,  141,  141,  141,  141,  141,  141,  141,  141,
 /*   860 */    26,   70,   71,   72,   30,  141,   75,   33,  141,  141,
 /*   870 */   141,  141,  141,  141,   40,   41,   42,   43,  141,   45,
 /*   880 */    46,   47,   48,  141,   50,  141,  141,  141,   54,   55,
 /*   890 */    56,  141,  141,   59,  141,  141,  141,  141,  141,  141,
 /*   900 */   141,  141,  141,   26,   70,   71,   72,   30,  141,   75,
 /*   910 */    33,  141,    1,    2,    3,    4,  141,   40,   41,   42,
 /*   920 */    43,  141,   45,   46,   47,   48,  141,   50,  141,  141,
 /*   930 */   141,   54,   55,   56,  141,  141,   59,  141,    1,    2,
 /*   940 */     3,    4,  141,  141,   26,  141,   35,   70,   71,   72,
 /*   950 */    39,   33,   75,  141,    1,    2,    3,    4,   40,   41,
 /*   960 */    42,   43,  141,   45,   46,   47,   48,   30,   50,  141,
 /*   970 */    33,  141,   54,   55,   56,  141,   39,   59,   87,  141,
 /*   980 */   141,  141,  141,  141,  141,  141,   34,  141,   70,   71,
 /*   990 */    72,  100,   39,   75,  141,  141,  105,  106,  107,  108,
 /*  1000 */   109,  110,  111,  112,  113,  114,  115,  116,  117,  118,
 /*  1010 */   119,   87,   60,   61,   62,   63,   64,   65,   66,  141,
 /*  1020 */    68,   69,  131,  141,  100,  141,  141,  141,  141,  105,
 /*  1030 */   106,  107,  108,  109,  110,  111,  112,  113,  114,  115,
 /*  1040 */   116,  117,  118,  119,  141,  141,   87,   81,   82,   83,
 /*  1050 */    84,   85,   86,   87,  141,  131,  141,  141,  141,  100,
 /*  1060 */   141,  141,  141,  141,  105,  106,  107,  108,  109,  110,
 /*  1070 */   111,  112,  113,  114,  115,  116,  117,  118,  119,   87,
 /*  1080 */    90,   91,   92,   93,  141,   95,   96,   97,   98,  141,
 /*  1090 */   131,  101,  100,  141,  141,  141,  141,  105,  106,  107,
 /*  1100 */   108,  109,  110,  111,  112,  113,  114,  115,  116,  117,
 /*  1110 */   118,  119,   87,  141,   91,   92,   93,  141,   95,  141,
 /*  1120 */    97,   98,  141,  131,  101,  100,    1,    2,    3,    4,
 /*  1130 */   105,  106,  107,  108,  109,  110,  111,  112,  113,  114,
 /*  1140 */   115,  116,  117,  118,  119,  141,  141,   87,  141,    1,
 /*  1150 */     2,    3,    4,  141,  141,   30,  131,  141,   33,  141,
 /*  1160 */   100,  141,  141,  141,   39,  105,  106,  107,  108,  109,
 /*  1170 */   110,  111,  112,  113,  114,  115,  116,  117,  118,  119,
 /*  1180 */    87,  141,  141,   35,  141,  141,  141,   39,  141,  141,
 /*  1190 */   141,  131,  141,  100,  141,  141,  141,  141,  105,  106,
 /*  1200 */   107,  108,  109,  110,  111,  112,  113,  114,  115,  116,
 /*  1210 */   117,  118,  119,   87,  141,   91,  141,  141,  141,  141,
 /*  1220 */    96,   97,   98,  141,  131,  101,  100,  141,  141,  141,
 /*  1230 */   141,  105,  106,  107,  108,  109,  110,  111,  112,  113,
 /*  1240 */   114,  115,  116,  117,  118,  119,  141,  141,   87,  141,
 /*  1250 */   141,  141,  141,  141,  141,  141,  141,  131,  141,  141,
 /*  1260 */   141,  100,  141,  141,  141,  141,  105,  106,  107,  108,
 /*  1270 */   109,  110,  111,  112,  113,  114,  115,  116,  117,  118,
 /*  1280 */   119,   87,  141,  141,  141,  141,  141,  141,  141,  141,
 /*  1290 */   141,  141,  131,  141,  100,  141,  141,  141,  141,  105,
 /*  1300 */   106,  107,  108,  109,  110,  111,  112,  113,  114,  115,
 /*  1310 */   116,  117,  118,  119,   87,  141,  141,  141,  141,  141,
 /*  1320 */   141,  141,  141,  141,  141,  131,  141,  100,  141,  141,
 /*  1330 */   141,  141,  105,  106,  107,  108,  109,  110,  111,  112,
 /*  1340 */   113,  114,  115,  116,  117,  118,  119,  141,  141,   87,
 /*  1350 */   141,  141,  141,  141,  141,  141,  141,  141,  131,  141,
 /*  1360 */   141,  141,  100,  141,  141,  141,  141,  105,  106,  107,
 /*  1370 */   108,  109,  110,  111,  112,  113,  114,  115,  116,  117,
 /*  1380 */   118,  119,   87,  141,  141,  141,  141,  141,  141,  141,
 /*  1390 */   141,  141,  141,  131,  141,  100,  141,  141,  141,  141,
 /*  1400 */   105,  106,  107,  108,  109,  110,  111,  112,  113,  114,
 /*  1410 */   115,  116,  117,  118,  119,   87,  141,  141,  141,  141,
 /*  1420 */   141,  141,  141,  141,  141,  141,  131,  141,  100,  141,
 /*  1430 */   141,  141,  141,  105,  106,  107,  108,  109,  110,  111,
 /*  1440 */   112,  113,  114,  115,  116,  117,  118,  119,  141,  141,
 /*  1450 */    87,  141,  141,  141,  141,  141,  141,  141,  141,  131,
 /*  1460 */   141,  141,  141,  100,  141,  141,  141,  141,  105,  106,
 /*  1470 */   107,  108,  109,  110,  111,  112,  113,  114,  115,  116,
 /*  1480 */   117,  118,  119,   87,  141,  141,  141,  141,  141,  141,
 /*  1490 */   141,  141,  141,  141,  131,  141,  141,  141,  141,  141,
 /*  1500 */   141,  105,  106,  107,  108,  109,  110,  111,  112,  113,
 /*  1510 */   114,  115,  116,  117,  118,  119,  141,  141,  141,  141,
 /*  1520 */   141,   19,   20,   21,   22,   23,   24,  131,   26,  141,
 /*  1530 */    19,   20,   21,   22,   23,   24,  141,   26,  141,   37,
 /*  1540 */   141,  141,  141,  141,  141,  141,  141,  141,   37,   38,
 /*  1550 */   141,  141,   19,   20,   21,   22,   23,   24,  141,   26,
 /*  1560 */   141,  141,  141,  141,  141,  141,  141,  141,   66,   67,
 /*  1570 */    37,   38,  141,  141,  141,   73,   74,   66,   76,   77,
 /*  1580 */    78,   79,  141,  141,   73,   74,  141,   76,   77,   78,
 /*  1590 */    79,  141,  141,  141,  141,  141,  141,  141,  141,   66,
 /*  1600 */   141,  141,  141,  141,  141,  141,   73,   74,  141,   76,
 /*  1610 */    77,   78,   79,  141,  141,  141,  141,  141,  141,   19,
 /*  1620 */    20,   21,   22,   23,   24,  141,   26,   89,  141,   91,
 /*  1630 */    92,   93,  141,   95,   96,   97,   98,   37,  141,  101,
 /*  1640 */   141,   19,   20,   21,   22,   23,   24,  141,   26,  141,
 /*  1650 */   141,  141,   10,  141,  141,  141,  141,  141,   58,   37,
 /*  1660 */    38,  141,  141,  141,  141,  141,   66,  141,   26,  141,
 /*  1670 */   141,  141,  141,   73,   74,  141,   76,   77,   78,   79,
 /*  1680 */    38,  141,   40,   41,   42,   43,   44,   45,   66,   10,
 /*  1690 */   141,  141,  141,  141,  141,   73,   74,  141,   76,   77,
 /*  1700 */    78,   79,  141,  141,  141,   26,  141,  141,   19,   20,
 /*  1710 */    21,   22,   23,   24,  141,   26,  141,   38,  141,   40,
 /*  1720 */    41,   42,   43,   44,   45,  141,   37,  141,  141,  141,
 /*  1730 */    19,   20,   21,   22,   23,   24,  141,   26,  141,  141,
 /*  1740 */   141,   10,  141,  141,  141,  141,  141,  141,   37,  141,
 /*  1750 */   141,  141,  141,  141,  141,   66,   67,   26,  141,  141,
 /*  1760 */   141,  141,   73,   74,  141,   76,   77,   78,   79,   58,
 /*  1770 */   141,   40,   41,   42,   43,   44,   45,   66,    1,    2,
 /*  1780 */     3,    4,  141,  141,   73,   74,  141,   76,   77,   78,
 /*  1790 */    79,  141,  141,  141,  141,  141,  141,   19,   20,   21,
 /*  1800 */    22,   23,   24,  141,   26,  141,  141,   30,  141,  141,
 /*  1810 */    33,  141,   35,  141,  141,   37,   39,  141,  141,  141,
 /*  1820 */   141,  141,    6,    7,    8,    9,   10,   11,   12,   13,
 /*  1830 */    14,   15,   16,   17,   18,  141,  141,  141,  141,  141,
 /*  1840 */   141,  141,  141,  141,   66,  141,  141,  141,  141,  141,
 /*  1850 */   141,   73,   74,  141,   76,   77,   78,   79,    6,    7,
 /*  1860 */     8,    9,   10,   11,   12,   13,   14,   15,   16,   17,
 /*  1870 */    18,    8,    9,   10,   11,   12,   13,   14,   15,   16,
 /*  1880 */    17,   18,  141,   67,  141,    1,    2,    3,    4,  141,
 /*  1890 */    38,  141,  141,  141,  141,  141,  141,    6,    7,    8,
 /*  1900 */     9,   10,   11,   12,   13,   14,   15,   16,   17,   18,
 /*  1910 */   141,  141,  141,  141,   30,  141,  141,   33,  141,   35,
 /*  1920 */    29,  141,  141,   39,  141,  141,  141,    6,    7,    8,
 /*  1930 */     9,   10,   11,   12,   13,   14,   15,   16,   17,   18,
 /*  1940 */   141,    1,    2,    3,    4,    1,    2,    3,    4,  141,
 /*  1950 */    29,  141,  141,  141,  141,  141,  141,    6,    7,    8,
 /*  1960 */     9,   10,   11,   12,   13,   14,   15,   16,   17,   18,
 /*  1970 */    30,  141,  141,   33,   30,   35,  141,   33,  141,   39,
 /*  1980 */    29,  141,  141,   39,  141,  141,  141,    6,    7,    8,
 /*  1990 */     9,   10,   11,   12,   13,   14,   15,   16,   17,   18,
 /*  2000 */   141,  141,  141,  141,  141,  141,  141,  141,  141,  141,
 /*  2010 */    29,  141,  141,  141,  141,  141,  141,    6,    7,    8,
 /*  2020 */     9,   10,   11,   12,   13,   14,   15,   16,   17,   18,
 /*  2030 */   141,    1,    2,    3,    4,    1,    2,    3,    4,  141,
 /*  2040 */    29,  141,  141,  141,  141,  141,  141,    6,    7,    8,
 /*  2050 */     9,   10,   11,   12,   13,   14,   15,   16,   17,   18,
 /*  2060 */    30,  141,  141,   33,   30,   35,  141,   33,   27,   39,
 /*  2070 */   141,  141,  141,   39,  141,    6,    7,    8,    9,   10,
 /*  2080 */    11,   12,   13,   14,   15,   16,   17,   18,   88,   89,
 /*  2090 */    90,   91,   92,   93,  141,   95,   96,   97,   98,  141,
 /*  2100 */   141,  101,  141,  141,  141,  141,   88,   89,   90,   91,
 /*  2110 */    92,   93,  141,   95,   96,   97,   98,  141,  141,  101,
 /*  2120 */   141,    1,    2,    3,    4,    1,    2,    3,    4,  141,
 /*  2130 */   141,  141,  141,  141,  141,  141,   88,   89,   90,   91,
 /*  2140 */    92,   93,  141,   95,   96,   97,   98,  141,  141,  101,
 /*  2150 */    30,  141,  141,   33,   30,   35,  141,   33,  141,   39,
 /*  2160 */   141,  141,  141,   39,   88,   89,   90,   91,   92,   93,
 /*  2170 */   141,   95,   96,   97,   98,  141,  141,  101,  141,    1,
 /*  2180 */     2,    3,    4,  141,  141,  141,  141,  141,  141,  141,
 /*  2190 */   141,  141,  141,  141,  141,  141,  141,  141,  141,  141,
 /*  2200 */   141,  141,  141,  141,  141,  141,  141,  141,   30,  141,
 /*  2210 */   141,   33,  141,   35,  141,  141,  141,   39,  141,    1,
 /*  2220 */     2,    3,    4,    1,    2,    3,    4,  141,  141,  141,
 /*  2230 */   141,  141,  141,  141,  141,  141,  141,  141,  141,  141,
 /*  2240 */   141,  141,  141,  141,  141,  141,  141,  141,   30,  141,
 /*  2250 */   141,   33,   30,   35,  141,   33,  141,   39,  141,  141,
 /*  2260 */   141,   39,
};
#define YY_SHIFT_USE_DFLT (-27)
static short yy_shift_ofst[] = {
 /*     0 */   152,  726,   66,  -27,  -27,  -27,  -27,  743,  340,  -27,
 /*    10 */   415,   54, 1777,  633,  -27,  -27, 2218, 2034,  -27,  694,
 /*    20 */   699,  686, 1679,  642,  877,  -27,  103,  -27,  -27,  -27,
 /*    30 */   -27,  -27,  -27,  -27,  -27,  -27,  -27,  -27,  -27,  -27,
 /*    40 */   -27,  -27,  -27,  -27,  712,  -27,  697,  -27, 1778,  652,
 /*    50 */   533,  232,  631,  648,  146,  447,  -27, 1778,  567,  345,
 /*    60 */   -27,  332,  -27,  -27,  130,  747,  918,  918,  677,   60,
 /*    70 */   189,  -27, 1778,  690,  834,  748,  -27,  715,  142, 1711,
 /*    80 */  2011,  918,  -26,  -27,  687,  477,  -27,  687,  -27,  952,
 /*    90 */   -27,  -27,  -27,  -27,  -27,  -27, 1778,  -27, 2069, 1778,
 /*   100 */  1778,  -27, 1778,  587,  -27,  488,  108,  399,  -27,  143,
 /*   110 */   -27,  719,  -27,  -15, 1778,  -15, 1778,  -15, 1778,  -15,
 /*   120 */  1778,  -15, 1778,  -15, 1778,  -15, 1778,  -27, 1778,  -27,
 /*   130 */  1778,  -27, 1778, 1863, 1778, 1863, 1778, 1852,  -27,  102,
 /*   140 */   672,  647, 1622, 2069, 1778,  -27,  746,  751,  587,  -27,
 /*   150 */  1778,  -27,  -27,  -27,  -27,  -27,  -27,  -27,    1,  -27,
 /*   160 */   261,  -27,   87,  -27,  645,   44,  -27,  -27,  -27,  -27,
 /*   170 */   -27,  -27,  -27,  -27,  -27,  -27,  616,  604,  602,  -27,
 /*   180 */   615,  603, 1533,  -27,  418,  -27, 1778,  -27,  -27,  -27,
 /*   190 */   433,  -27,  -27,  366,  720,  -27,  706,  -27,  701,  -27,
 /*   200 */  1511,  519,  -27,  -27,  -15,  -20,  -25, 1778,  -27, 1689,
 /*   210 */    -9, 1778,  -27,  528,   -9, 1778,  -27, 1816, 1502,   -9,
 /*   220 */  1778,  -27,  609,   -9, 1778,  -27,  -27,  -27,  -27, 1778,
 /*   230 */   593,  -27, 1778,  -27, 2069,  -27,  738,  -27, 1778, 2041,
 /*   240 */   -27,  716,  444,  667,  740,  -27,  326,  503,  739,  -27,
 /*   250 */   632,  611,  -27,  405,  125,  -27,  -18,   13,  -27,  732,
 /*   260 */   717, 1778,   18,  722,  -27,  696,  223,  -27,  696,  -27,
 /*   270 */   693,  130,  -27,  -27,  696,  437,  -27,  696,  438,  -27,
 /*   280 */   696,  464,  -27,  696,  478,  -27,  655,  688,  647,  -27,
 /*   290 */   -27, 1778, 1891,  918,  705,  -27,  670,  630, 1600, 1921,
 /*   300 */   918,  619,  -27, 1778, 1951,  918,  275,  -27,  -27, 1981,
 /*   310 */   -27,  620,  918,  918,  -27,  -27,  621,  628,  361,  490,
 /*   320 */   -27,  -27,  374,  613,  791,  -27,   17,  -27, 1731,  -27,
 /*   330 */   605,  130,  -27,  588,  578,  130,  -27,  573,  524,  130,
 /*   340 */   -27,  515,  474,  460,  442,  384,  371,  360,  314,  272,
 /*   350 */   -27,  210,  770,  758,  768, 1642,  750,  318,  -27,  404,
 /*   360 */   -27,  491,  741,  576,  -27,  662,  -27,  727,  392,  735,
 /*   370 */    45,  749,  756,  335,  761,  764,  765,  445,  772,   75,
 /*   380 */   352,  147,  161, 2120, 2124,  -27,  -27, 1148,  472,  493,
 /*   390 */   130,  526,  -27,  542,  537,  130,  570,  -27,  -27,  911,
 /*   400 */   250,  428,  -27,  130,  617,  -27,  -27,  -27,  -27,  -27,
 /*   410 */   -27,  -27,  646,  476,  650,  489,  266,  557,  -27,  130,
 /*   420 */   684,  -27,  -27, 2178, 1125, 1944,  953, 2222,  -27,  937,
 /*   430 */   710,  711, 1884,  714,  -27,  -27,  729,  736,  351, 1940,
 /*   440 */   752,  -27,  -27,  627,  654, 2030,  685,  -27,  -27,  -27,
};
#define YY_REDUCE_USE_DFLT (-106)
static short yy_reduce_ofst[] = {
 /*     0 */   966, -106,  -74, -106, -106, -106, -106, -106, -106, -106,
 /*    10 */  -106, -106, 2000, -106, -106, -106,  990,  199, -106, -106,
 /*    20 */  -106, -106,   38, -106,  924, -106, 1396, -106, -106, -106,
 /*    30 */  -106, -106, -106, -106, -106, -106, -106, -106, -106, -106,
 /*    40 */  -106, -106, -106, -106, -106, -106, -106, -106,  -89, -106,
 /*    50 */  1363, 1396, -106, -106, 1262, 1396, -106,  347, -106,  248,
 /*    60 */  -106,   16, -106, -106,  689, -106, 1295, 1396, -106, 1161,
 /*    70 */  1396, -106,  304, -106, 1194, 1396, -106, -106, -106,  188,
 /*    80 */  -106, 1227, 1396, -106,  159, -106, -106,  575, -106,  559,
 /*    90 */  -106, -106, -106, -106, -106, -106,  295, -106, -106,  403,
 /*   100 */   416, -106,  419,  465, -106, -106, -106, -106, -106, -106,
 /*   110 */  -106, -106, -106, -106,  420, -106,  425, -106,  441, -106,
 /*   120 */   446, -106,  459, -106,  462, -106,  463, -106,  484, -106,
 /*   130 */   502, -106,  505, -106,  527, -106,  -62, -106, -106, -106,
 /*   140 */  -106, -106,  282, -106,  124, -106, -106, -106,  608, -106,
 /*   150 */   140, -106, -106, -106, -106, -106, -106, -106,  -12, -106,
 /*   160 */  -106, -106,   31, -106, -106,  215, -106, -106, -106, -106,
 /*   170 */  -106, -106, -106, -106, -106, -106, -106, -106, -106, -106,
 /*   180 */  -106, -106,  162, -106, -106, -106, -105, -106, -106, -106,
 /*   190 */  -106, -106, -106, -106, -106, -106, -106, -106, -106, -106,
 /*   200 */   158, -106, -106, -106, -106, -106,   30,  183, -106,  -19,
 /*   210 */   426,  115, -106, -106,  485,  298, -106, -106,   67,  529,
 /*   220 */   226, -106, -106,  553,  330, -106, -106, -106, -106,  212,
 /*   230 */  -106, -106,  110, -106, -106, -106, -106, -106,  506, -106,
 /*   240 */  -106, -106, -106, -106, -106, -106, -106, -106, -106, -106,
 /*   250 */  -106, -106, -106, -106, -106, -106, -106, -106, -106, -106,
 /*   260 */  -106,  201, -106, -106, -106,   21, -106, -106,  595, -106,
 /*   270 */  -106,  634, -106, -106,   93, -106, -106,  123, -106, -106,
 /*   280 */   179, -106, -106,  193, -106, -106, -106, -106, -106, -106,
 /*   290 */  -106,  376, -106,  959, 1396, -106, -106, -106,  377, -106,
 /*   300 */   992, 1396, -106,  390, -106, 1093, 1396, -106, -106, -106,
 /*   310 */  -106, -106, 1060, 1396, -106, -106, -106, -106,  891, 1396,
 /*   320 */  -106, -106, -106, -106, 1328, -106, 1396, -106,   -7, -106,
 /*   330 */  -106,  540, -106, -106, -106,  516, -106, -106, -106,  473,
 /*   340 */  -106, -106, -106, -106, -106, -106, -106, -106, -106, -106,
 /*   350 */  -106,  413, -106, -106, -106,  121, -106, 1025, -106, 1396,
 /*   360 */  -106, -106, -106, 1126, -106, 1396, -106, -106, -106, -106,
 /*   370 */  -106, -106, -106, -106, -106, -106, -106, -106, -106, -106,
 /*   380 */  -106, -106, -106, 1124,  199, -106, -106,  413, -106, -106,
 /*   390 */   461, -106, -106, -106, -106,  507, -106, -106, -106,  283,
 /*   400 */   532, -106, -106,  549, -106, -106, -106, -106, -106, -106,
 /*   410 */  -106, -106, -106, -106, -106, -106,  532, -106, -106,  607,
 /*   420 */  -106, -106, -106, 1538, 1023,  199,  283,  199, -106,  199,
 /*   430 */  -106, -106, 2048, -106, -106, -106, -106, -106, -106, 2076,
 /*   440 */  -106, -106, -106, -106, -106, 2018, -106, -106, -106, -106,
};
static YYACTIONTYPE yy_default[] = {
 /*     0 */   673,  673,  673,  452,  454,  455,  456,  673,  673,  457,
 /*    10 */   673,  673,  673,  673,  458,  462,  673,  673,  489,  673,
 /*    20 */   673,  673,  673,  673,  673,  493,  673,  495,  531,  533,
 /*    30 */   534,  535,  536,  537,  538,  539,  540,  541,  542,  543,
 /*    40 */   544,  545,  546,  547,  673,  548,  673,  549,  673,  673,
 /*    50 */   673,  673,  552,  673,  673,  673,  553,  673,  673,  673,
 /*    60 */   556,  673,  557,  558,  673,  673,  673,  560,  673,  673,
 /*    70 */   673,  563,  673,  673,  673,  673,  565,  673,  673,  673,
 /*    80 */   673,  673,  673,  566,  673,  673,  570,  673,  571,  673,
 /*    90 */   573,  574,  575,  576,  577,  578,  673,  579,  609,  673,
 /*   100 */   673,  610,  673,  673,  611,  673,  673,  673,  668,  673,
 /*   110 */   669,  673,  670,  613,  673,  614,  673,  615,  673,  616,
 /*   120 */   673,  617,  673,  618,  673,  619,  673,  625,  673,  626,
 /*   130 */   673,  627,  673,  628,  673,  629,  673,  673,  620,  633,
 /*   140 */   673,  621,  673,  652,  673,  630,  673,  673,  673,  631,
 /*   150 */   673,  632,  634,  635,  636,  637,  638,  639,  673,  640,
 /*   160 */   673,  641,  673,  653,  673,  673,  655,  660,  661,  662,
 /*   170 */   663,  664,  665,  666,  667,  656,  661,  663,  662,  654,
 /*   180 */   673,  642,  673,  643,  673,  644,  673,  647,  650,  651,
 /*   190 */   673,  648,  649,  673,  673,  622,  673,  623,  673,  624,
 /*   200 */   673,  673,  645,  646,  612,  673,  673,  673,  580,  673,
 /*   210 */   673,  673,  581,  673,  673,  673,  582,  673,  673,  673,
 /*   220 */   673,  583,  673,  673,  673,  584,  585,  586,  572,  673,
 /*   230 */   673,  588,  673,  589,  591,  590,  673,  592,  673,  673,
 /*   240 */   593,  673,  673,  673,  673,  594,  673,  673,  673,  595,
 /*   250 */   673,  673,  596,  673,  673,  597,  673,  673,  598,  673,
 /*   260 */   673,  673,  673,  673,  599,  673,  673,  600,  673,  605,
 /*   270 */   607,  673,  608,  606,  673,  673,  601,  673,  673,  602,
 /*   280 */   673,  673,  603,  673,  673,  604,  673,  673,  673,  672,
 /*   290 */   532,  673,  673,  673,  673,  567,  673,  673,  673,  673,
 /*   300 */   673,  673,  568,  673,  673,  673,  673,  569,  564,  673,
 /*   310 */   562,  673,  673,  561,  559,  554,  550,  673,  673,  673,
 /*   320 */   555,  551,  673,  673,  673,  494,  673,  496,  673,  516,
 /*   330 */   518,  673,  521,  673,  519,  673,  522,  673,  520,  673,
 /*   340 */   523,  673,  673,  673,  673,  673,  673,  673,  673,  673,
 /*   350 */   517,  673,  673,  673,  673,  673,  673,  673,  497,  673,
 /*   360 */   499,  673,  673,  673,  498,  673,  500,  673,  673,  673,
 /*   370 */   673,  673,  673,  673,  673,  673,  673,  673,  673,  673,
 /*   380 */   673,  673,  673,  673,  673,  487,  490,  673,  673,  673,
 /*   390 */   673,  673,  491,  673,  673,  673,  673,  492,  475,  673,
 /*   400 */   673,  673,  477,  673,  673,  479,  481,  483,  484,  485,
 /*   410 */   486,  482,  483,  484,  485,  486,  673,  673,  478,  673,
 /*   420 */   673,  480,  488,  673,  673,  673,  673,  673,  476,  673,
 /*   430 */   673,  673,  673,  673,  459,  463,  673,  673,  673,  673,
 /*   440 */   673,  460,  464,  673,  673,  673,  673,  461,  465,  453,
};
#define YY_SZ_ACTTAB (sizeof(yy_action)/sizeof(yy_action[0]))

/* The next table maps tokens into fallback tokens.  If a construct
** like the following:
** 
**      %fallback ID X Y Z.
**
** appears in the grammer, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
*/
#ifdef YYFALLBACK
static const YYCODETYPE yyFallback[] = {
};
#endif /* YYFALLBACK */

/* The following structure represents a single element of the
** parser's stack.  Information stored includes:
**
**   +  The state number for the parser at this level of the stack.
**
**   +  The value of the token stored at this level of the stack.
**      (In other words, the "major" token.)
**
**   +  The semantic value stored at this level of the stack.  This is
**      the information used by the action routines in the grammar.
**      It is sometimes called the "minor" token.
*/
struct yyStackEntry {
  int stateno;       /* The state-number */
  int major;         /* The major token value.  This is the code
                     ** number for the token at this stack level */
  YYMINORTYPE minor; /* The user-supplied minor token value.  This
                     ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser {
  int yyidx;                    /* Index of top element in stack */
  int yyerrcnt;                 /* Shifts left before out of the error */
  xx_ARG_SDECL                /* A place to hold %extra_argument */
  yyStackEntry yystack[YYSTACKDEPTH];  /* The parser's stack */
};
typedef struct yyParser yyParser;

#ifndef NDEBUG
#include <stdio.h>
static FILE *yyTraceFILE = 0;
static char *yyTracePrompt = 0;
#endif /* NDEBUG */

#ifndef NDEBUG
/* 
** Turn parser tracing on by giving a stream to which to write the trace
** and a prompt to preface each trace message.  Tracing is turned off
** by making either argument NULL 
**
** Inputs:
** <ul>
** <li> A FILE* to which trace output should be written.
**      If NULL, then tracing is turned off.
** <li> A prefix string written at the beginning of every
**      line of trace output.  If NULL, then tracing is
**      turned off.
** </ul>
**
** Outputs:
** None.
*/
void xx_Trace(FILE *TraceFILE, char *zTracePrompt){
  yyTraceFILE = TraceFILE;
  yyTracePrompt = zTracePrompt;
  if( yyTraceFILE==0 ) yyTracePrompt = 0;
  else if( yyTracePrompt==0 ) yyTraceFILE = 0;
}
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *yyTokenName[] = { 
  "$",             "PUBLIC",        "PROTECTED",     "STATIC",      
  "PRIVATE",       "COMMA",         "AND",           "OR",          
  "EQUALS",        "IDENTICAL",     "LESS",          "GREATER",     
  "LESSEQUAL",     "GREATEREQUAL",  "NOTIDENTICAL",  "NOTEQUALS",   
  "ADD",           "SUB",           "CONCAT",        "ISSET",       
  "FETCH",         "TYPEOF",        "INSTANCEOF",    "NEW",         
  "NOT",           "NAMESPACE",     "IDENTIFIER",    "DOTCOMMA",    
  "CLASS",         "BRACKET_OPEN",  "BRACKET_CLOSE",  "EXTENDS",     
  "ABSTRACT",      "COMMENT",       "ASSIGN",        "CONST",       
  "FUNCTION",      "PARENTHESES_OPEN",  "PARENTHESES_CLOSE",  "INLINE",      
  "TYPE_INTEGER",  "TYPE_DOUBLE",   "TYPE_BOOL",     "TYPE_STRING", 
  "TYPE_ARRAY",    "TYPE_VAR",      "BREAK",         "CONTINUE",    
  "IF",            "ELSE",          "SWITCH",        "CASE",        
  "COLON",         "DEFAULT",       "LOOP",          "WHILE",       
  "FOR",           "IN",            "REVERSE",       "LET",         
  "ADDASSIGN",     "SUBASSIGN",     "MULASSIGN",     "DIVASSIGN",   
  "CONCATASSIGN",  "ARROW",         "SBRACKET_OPEN",  "SBRACKET_CLOSE",
  "INCR",          "DECR",          "ECHO",          "RETURN",      
  "UNSET",         "INTEGER",       "STRING",        "THROW",       
  "DOUBLE",        "NULL",          "TRUE",          "FALSE",       
  "error",         "program",       "xx_language",   "xx_top_statement_list",
  "xx_top_statement",  "xx_namespace_def",  "xx_class_def",  "xx_comment",  
  "xx_class_definition",  "xx_class_properties_definition",  "xx_class_consts_definition",  "xx_class_methods_definition",
  "xx_class_property_definition",  "xx_property_visibility_list",  "xx_literal_expr",  "xx_property_visibility",
  "xx_class_const_definition",  "xx_class_method_definition",  "xx_method_visibility_list",  "xx_parameter_list",
  "xx_statement_list",  "xx_method_visibility",  "xx_parameter",  "xx_parameter_type",
  "xx_parameter_cast",  "xx_statement",  "xx_let_statement",  "xx_if_statement",
  "xx_loop_statement",  "xx_echo_statement",  "xx_return_statement",  "xx_mcall_statement",
  "xx_unset_statement",  "xx_throw_statement",  "xx_declare_statement",  "xx_break_statement",
  "xx_continue_statement",  "xx_while_statement",  "xx_switch_statement",  "xx_for_statement",
  "xx_eval_expr",  "xx_case_clauses",  "xx_case_clause",  "xx_common_expr",
  "xx_let_assignments",  "xx_let_assignment",  "xx_assignment_operator",  "xx_assign_expr",
  "xx_index_expr",  "xx_echo_expressions",  "xx_echo_expression",  "xx_mcall_expr",
  "xx_call_parameters",  "xx_declare_variable_list",  "xx_declare_variable",  "xx_isset_expr",
  "xx_array_list",  "xx_call_parameter",  "xx_array_item",  "xx_array_key",
  "xx_array_value",
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *yyRuleName[] = {
 /*   0 */ "program ::= xx_language",
 /*   1 */ "xx_language ::= xx_top_statement_list",
 /*   2 */ "xx_top_statement_list ::= xx_top_statement_list xx_top_statement",
 /*   3 */ "xx_top_statement_list ::= xx_top_statement",
 /*   4 */ "xx_top_statement ::= xx_namespace_def",
 /*   5 */ "xx_top_statement ::= xx_class_def",
 /*   6 */ "xx_top_statement ::= xx_comment",
 /*   7 */ "xx_namespace_def ::= NAMESPACE IDENTIFIER DOTCOMMA",
 /*   8 */ "xx_class_def ::= CLASS IDENTIFIER BRACKET_OPEN xx_class_definition BRACKET_CLOSE",
 /*   9 */ "xx_class_def ::= CLASS IDENTIFIER EXTENDS IDENTIFIER BRACKET_OPEN xx_class_definition BRACKET_CLOSE",
 /*  10 */ "xx_class_def ::= ABSTRACT CLASS IDENTIFIER BRACKET_OPEN xx_class_definition BRACKET_CLOSE",
 /*  11 */ "xx_class_def ::= ABSTRACT CLASS IDENTIFIER EXTENDS IDENTIFIER BRACKET_OPEN xx_class_definition BRACKET_CLOSE",
 /*  12 */ "xx_class_def ::= CLASS IDENTIFIER BRACKET_OPEN BRACKET_CLOSE",
 /*  13 */ "xx_class_def ::= CLASS IDENTIFIER EXTENDS IDENTIFIER BRACKET_OPEN BRACKET_CLOSE",
 /*  14 */ "xx_class_def ::= ABSTRACT CLASS IDENTIFIER BRACKET_OPEN BRACKET_CLOSE",
 /*  15 */ "xx_class_def ::= ABSTRACT CLASS IDENTIFIER EXTENDS IDENTIFIER BRACKET_OPEN BRACKET_CLOSE",
 /*  16 */ "xx_class_definition ::= xx_class_properties_definition",
 /*  17 */ "xx_class_definition ::= xx_class_consts_definition",
 /*  18 */ "xx_class_definition ::= xx_class_methods_definition",
 /*  19 */ "xx_class_definition ::= xx_class_properties_definition xx_class_methods_definition",
 /*  20 */ "xx_class_definition ::= xx_class_properties_definition xx_class_consts_definition",
 /*  21 */ "xx_class_definition ::= xx_class_consts_definition xx_class_properties_definition",
 /*  22 */ "xx_class_definition ::= xx_class_consts_definition xx_class_methods_definition",
 /*  23 */ "xx_class_definition ::= xx_class_properties_definition xx_class_consts_definition xx_class_methods_definition",
 /*  24 */ "xx_class_definition ::= xx_class_consts_definition xx_class_properties_definition xx_class_methods_definition",
 /*  25 */ "xx_class_properties_definition ::= xx_class_properties_definition xx_class_property_definition",
 /*  26 */ "xx_class_properties_definition ::= xx_class_property_definition",
 /*  27 */ "xx_class_property_definition ::= COMMENT xx_property_visibility_list IDENTIFIER DOTCOMMA",
 /*  28 */ "xx_class_property_definition ::= xx_property_visibility_list IDENTIFIER DOTCOMMA",
 /*  29 */ "xx_class_property_definition ::= COMMENT xx_property_visibility_list IDENTIFIER ASSIGN xx_literal_expr DOTCOMMA",
 /*  30 */ "xx_class_property_definition ::= xx_property_visibility_list IDENTIFIER ASSIGN xx_literal_expr DOTCOMMA",
 /*  31 */ "xx_property_visibility_list ::= xx_property_visibility_list xx_property_visibility",
 /*  32 */ "xx_property_visibility_list ::= xx_property_visibility",
 /*  33 */ "xx_property_visibility ::= PUBLIC",
 /*  34 */ "xx_property_visibility ::= PROTECTED",
 /*  35 */ "xx_property_visibility ::= STATIC",
 /*  36 */ "xx_property_visibility ::= PRIVATE",
 /*  37 */ "xx_class_consts_definition ::= xx_class_consts_definition xx_class_const_definition",
 /*  38 */ "xx_class_consts_definition ::= xx_class_const_definition",
 /*  39 */ "xx_class_methods_definition ::= xx_class_methods_definition xx_class_method_definition",
 /*  40 */ "xx_class_methods_definition ::= xx_class_method_definition",
 /*  41 */ "xx_class_const_definition ::= COMMENT CONST IDENTIFIER ASSIGN xx_literal_expr DOTCOMMA",
 /*  42 */ "xx_class_const_definition ::= CONST IDENTIFIER ASSIGN xx_literal_expr DOTCOMMA",
 /*  43 */ "xx_class_method_definition ::= xx_method_visibility_list FUNCTION IDENTIFIER PARENTHESES_OPEN PARENTHESES_CLOSE BRACKET_OPEN BRACKET_CLOSE",
 /*  44 */ "xx_class_method_definition ::= xx_method_visibility_list FUNCTION IDENTIFIER PARENTHESES_OPEN xx_parameter_list PARENTHESES_CLOSE BRACKET_OPEN BRACKET_CLOSE",
 /*  45 */ "xx_class_method_definition ::= xx_method_visibility_list FUNCTION IDENTIFIER PARENTHESES_OPEN PARENTHESES_CLOSE BRACKET_OPEN xx_statement_list BRACKET_CLOSE",
 /*  46 */ "xx_class_method_definition ::= xx_method_visibility_list FUNCTION IDENTIFIER PARENTHESES_OPEN xx_parameter_list PARENTHESES_CLOSE BRACKET_OPEN xx_statement_list BRACKET_CLOSE",
 /*  47 */ "xx_class_method_definition ::= COMMENT xx_method_visibility_list FUNCTION IDENTIFIER PARENTHESES_OPEN PARENTHESES_CLOSE BRACKET_OPEN BRACKET_CLOSE",
 /*  48 */ "xx_class_method_definition ::= COMMENT xx_method_visibility_list FUNCTION IDENTIFIER PARENTHESES_OPEN xx_parameter_list PARENTHESES_CLOSE BRACKET_OPEN BRACKET_CLOSE",
 /*  49 */ "xx_class_method_definition ::= COMMENT xx_method_visibility_list FUNCTION IDENTIFIER PARENTHESES_OPEN PARENTHESES_CLOSE BRACKET_OPEN xx_statement_list BRACKET_CLOSE",
 /*  50 */ "xx_class_method_definition ::= COMMENT xx_method_visibility_list FUNCTION IDENTIFIER PARENTHESES_OPEN xx_parameter_list PARENTHESES_CLOSE BRACKET_OPEN xx_statement_list BRACKET_CLOSE",
 /*  51 */ "xx_method_visibility_list ::= xx_method_visibility",
 /*  52 */ "xx_method_visibility ::= PUBLIC",
 /*  53 */ "xx_method_visibility ::= PROTECTED",
 /*  54 */ "xx_method_visibility ::= STATIC",
 /*  55 */ "xx_method_visibility ::= PRIVATE",
 /*  56 */ "xx_method_visibility ::= STATIC PUBLIC",
 /*  57 */ "xx_method_visibility ::= PUBLIC STATIC",
 /*  58 */ "xx_method_visibility ::= STATIC PROTECTED",
 /*  59 */ "xx_method_visibility ::= PROTECTED STATIC",
 /*  60 */ "xx_method_visibility ::= STATIC PRIVATE",
 /*  61 */ "xx_method_visibility ::= PRIVATE STATIC",
 /*  62 */ "xx_method_visibility ::= INLINE PROTECTED",
 /*  63 */ "xx_method_visibility ::= PROTECTED INLINE",
 /*  64 */ "xx_method_visibility ::= INLINE PRIVATE",
 /*  65 */ "xx_method_visibility ::= PRIVATE INLINE",
 /*  66 */ "xx_parameter_list ::= xx_parameter_list COMMA xx_parameter",
 /*  67 */ "xx_parameter_list ::= xx_parameter",
 /*  68 */ "xx_parameter ::= IDENTIFIER",
 /*  69 */ "xx_parameter ::= xx_parameter_type IDENTIFIER",
 /*  70 */ "xx_parameter ::= xx_parameter_cast IDENTIFIER",
 /*  71 */ "xx_parameter ::= IDENTIFIER ASSIGN xx_literal_expr",
 /*  72 */ "xx_parameter ::= xx_parameter_type IDENTIFIER ASSIGN xx_literal_expr",
 /*  73 */ "xx_parameter ::= xx_parameter_cast IDENTIFIER ASSIGN xx_literal_expr",
 /*  74 */ "xx_parameter_cast ::= LESS IDENTIFIER GREATER",
 /*  75 */ "xx_parameter_type ::= TYPE_INTEGER",
 /*  76 */ "xx_parameter_type ::= TYPE_DOUBLE",
 /*  77 */ "xx_parameter_type ::= TYPE_BOOL",
 /*  78 */ "xx_parameter_type ::= TYPE_STRING",
 /*  79 */ "xx_parameter_type ::= TYPE_ARRAY",
 /*  80 */ "xx_parameter_type ::= TYPE_VAR",
 /*  81 */ "xx_statement_list ::= xx_statement_list xx_statement",
 /*  82 */ "xx_statement_list ::= xx_statement",
 /*  83 */ "xx_statement ::= xx_let_statement",
 /*  84 */ "xx_statement ::= xx_if_statement",
 /*  85 */ "xx_statement ::= xx_loop_statement",
 /*  86 */ "xx_statement ::= xx_echo_statement",
 /*  87 */ "xx_statement ::= xx_return_statement",
 /*  88 */ "xx_statement ::= xx_mcall_statement",
 /*  89 */ "xx_statement ::= xx_unset_statement",
 /*  90 */ "xx_statement ::= xx_throw_statement",
 /*  91 */ "xx_statement ::= xx_declare_statement",
 /*  92 */ "xx_statement ::= xx_break_statement",
 /*  93 */ "xx_statement ::= xx_continue_statement",
 /*  94 */ "xx_statement ::= xx_while_statement",
 /*  95 */ "xx_statement ::= xx_switch_statement",
 /*  96 */ "xx_statement ::= xx_for_statement",
 /*  97 */ "xx_statement ::= xx_comment",
 /*  98 */ "xx_break_statement ::= BREAK DOTCOMMA",
 /*  99 */ "xx_continue_statement ::= CONTINUE DOTCOMMA",
 /* 100 */ "xx_if_statement ::= IF xx_eval_expr BRACKET_OPEN BRACKET_CLOSE",
 /* 101 */ "xx_if_statement ::= IF xx_eval_expr BRACKET_OPEN BRACKET_CLOSE ELSE BRACKET_OPEN BRACKET_CLOSE",
 /* 102 */ "xx_if_statement ::= IF xx_eval_expr BRACKET_OPEN xx_statement_list BRACKET_CLOSE",
 /* 103 */ "xx_if_statement ::= IF xx_eval_expr BRACKET_OPEN xx_statement_list BRACKET_CLOSE ELSE BRACKET_OPEN xx_statement_list BRACKET_CLOSE",
 /* 104 */ "xx_if_statement ::= IF xx_eval_expr BRACKET_OPEN xx_statement_list BRACKET_CLOSE ELSE BRACKET_OPEN BRACKET_CLOSE",
 /* 105 */ "xx_if_statement ::= IF xx_eval_expr BRACKET_OPEN BRACKET_CLOSE ELSE BRACKET_OPEN xx_statement_list BRACKET_CLOSE",
 /* 106 */ "xx_switch_statement ::= SWITCH xx_eval_expr BRACKET_OPEN BRACKET_CLOSE",
 /* 107 */ "xx_switch_statement ::= SWITCH xx_eval_expr BRACKET_OPEN xx_case_clauses BRACKET_CLOSE",
 /* 108 */ "xx_case_clauses ::= xx_case_clauses xx_case_clause",
 /* 109 */ "xx_case_clauses ::= xx_case_clause",
 /* 110 */ "xx_case_clause ::= CASE xx_literal_expr COLON xx_statement_list",
 /* 111 */ "xx_case_clause ::= DEFAULT COLON xx_statement_list",
 /* 112 */ "xx_loop_statement ::= LOOP BRACKET_OPEN BRACKET_CLOSE",
 /* 113 */ "xx_loop_statement ::= LOOP BRACKET_OPEN xx_statement_list BRACKET_CLOSE",
 /* 114 */ "xx_while_statement ::= WHILE xx_eval_expr BRACKET_OPEN BRACKET_CLOSE",
 /* 115 */ "xx_while_statement ::= WHILE xx_eval_expr BRACKET_OPEN xx_statement_list BRACKET_CLOSE",
 /* 116 */ "xx_for_statement ::= FOR IDENTIFIER IN xx_common_expr BRACKET_OPEN xx_statement_list BRACKET_CLOSE",
 /* 117 */ "xx_for_statement ::= FOR IDENTIFIER IN REVERSE xx_common_expr BRACKET_OPEN xx_statement_list BRACKET_CLOSE",
 /* 118 */ "xx_for_statement ::= FOR IDENTIFIER COMMA IDENTIFIER IN xx_common_expr BRACKET_OPEN xx_statement_list BRACKET_CLOSE",
 /* 119 */ "xx_for_statement ::= FOR IDENTIFIER COMMA IDENTIFIER IN REVERSE xx_common_expr BRACKET_OPEN xx_statement_list BRACKET_CLOSE",
 /* 120 */ "xx_let_statement ::= LET xx_let_assignments DOTCOMMA",
 /* 121 */ "xx_let_assignments ::= xx_let_assignments COMMA xx_let_assignment",
 /* 122 */ "xx_let_assignments ::= xx_let_assignment",
 /* 123 */ "xx_assignment_operator ::= ASSIGN",
 /* 124 */ "xx_assignment_operator ::= ADDASSIGN",
 /* 125 */ "xx_assignment_operator ::= SUBASSIGN",
 /* 126 */ "xx_assignment_operator ::= MULASSIGN",
 /* 127 */ "xx_assignment_operator ::= DIVASSIGN",
 /* 128 */ "xx_assignment_operator ::= CONCATASSIGN",
 /* 129 */ "xx_let_assignment ::= IDENTIFIER xx_assignment_operator xx_assign_expr",
 /* 130 */ "xx_let_assignment ::= IDENTIFIER ARROW IDENTIFIER xx_assignment_operator xx_assign_expr",
 /* 131 */ "xx_let_assignment ::= IDENTIFIER ARROW IDENTIFIER SBRACKET_OPEN SBRACKET_CLOSE xx_assignment_operator xx_assign_expr",
 /* 132 */ "xx_let_assignment ::= IDENTIFIER ARROW IDENTIFIER SBRACKET_OPEN xx_index_expr SBRACKET_CLOSE xx_assignment_operator xx_assign_expr",
 /* 133 */ "xx_let_assignment ::= IDENTIFIER SBRACKET_OPEN SBRACKET_CLOSE xx_assignment_operator xx_assign_expr",
 /* 134 */ "xx_let_assignment ::= IDENTIFIER SBRACKET_OPEN xx_index_expr SBRACKET_CLOSE xx_assignment_operator xx_assign_expr",
 /* 135 */ "xx_let_assignment ::= IDENTIFIER INCR",
 /* 136 */ "xx_let_assignment ::= IDENTIFIER DECR",
 /* 137 */ "xx_index_expr ::= xx_common_expr",
 /* 138 */ "xx_echo_statement ::= ECHO xx_echo_expressions DOTCOMMA",
 /* 139 */ "xx_echo_expressions ::= xx_echo_expressions COMMA xx_echo_expression",
 /* 140 */ "xx_echo_expressions ::= xx_echo_expression",
 /* 141 */ "xx_echo_expression ::= xx_common_expr",
 /* 142 */ "xx_mcall_statement ::= xx_mcall_expr DOTCOMMA",
 /* 143 */ "xx_return_statement ::= RETURN xx_common_expr DOTCOMMA",
 /* 144 */ "xx_unset_statement ::= UNSET IDENTIFIER ARROW IDENTIFIER DOTCOMMA",
 /* 145 */ "xx_unset_statement ::= UNSET IDENTIFIER SBRACKET_OPEN IDENTIFIER SBRACKET_CLOSE DOTCOMMA",
 /* 146 */ "xx_unset_statement ::= UNSET IDENTIFIER SBRACKET_OPEN INTEGER SBRACKET_CLOSE DOTCOMMA",
 /* 147 */ "xx_unset_statement ::= UNSET IDENTIFIER SBRACKET_OPEN STRING SBRACKET_CLOSE DOTCOMMA",
 /* 148 */ "xx_throw_statement ::= THROW IDENTIFIER DOTCOMMA",
 /* 149 */ "xx_throw_statement ::= THROW NEW IDENTIFIER PARENTHESES_OPEN xx_call_parameters PARENTHESES_CLOSE DOTCOMMA",
 /* 150 */ "xx_declare_statement ::= TYPE_INTEGER xx_declare_variable_list DOTCOMMA",
 /* 151 */ "xx_declare_statement ::= TYPE_DOUBLE xx_declare_variable_list DOTCOMMA",
 /* 152 */ "xx_declare_statement ::= TYPE_STRING xx_declare_variable_list DOTCOMMA",
 /* 153 */ "xx_declare_statement ::= TYPE_BOOL xx_declare_variable_list DOTCOMMA",
 /* 154 */ "xx_declare_statement ::= TYPE_VAR xx_declare_variable_list DOTCOMMA",
 /* 155 */ "xx_declare_variable_list ::= xx_declare_variable_list COMMA xx_declare_variable",
 /* 156 */ "xx_declare_variable_list ::= xx_declare_variable",
 /* 157 */ "xx_declare_variable ::= IDENTIFIER",
 /* 158 */ "xx_declare_variable ::= IDENTIFIER ASSIGN xx_literal_expr",
 /* 159 */ "xx_assign_expr ::= xx_common_expr",
 /* 160 */ "xx_common_expr ::= NOT xx_common_expr",
 /* 161 */ "xx_common_expr ::= ISSET xx_isset_expr",
 /* 162 */ "xx_common_expr ::= xx_common_expr EQUALS xx_common_expr",
 /* 163 */ "xx_common_expr ::= xx_common_expr NOTEQUALS xx_common_expr",
 /* 164 */ "xx_common_expr ::= xx_common_expr IDENTICAL xx_common_expr",
 /* 165 */ "xx_common_expr ::= xx_common_expr NOTIDENTICAL xx_common_expr",
 /* 166 */ "xx_common_expr ::= xx_common_expr LESS xx_common_expr",
 /* 167 */ "xx_common_expr ::= xx_common_expr GREATER xx_common_expr",
 /* 168 */ "xx_common_expr ::= xx_common_expr LESSEQUAL xx_common_expr",
 /* 169 */ "xx_common_expr ::= xx_common_expr GREATEREQUAL xx_common_expr",
 /* 170 */ "xx_common_expr ::= PARENTHESES_OPEN xx_common_expr PARENTHESES_CLOSE",
 /* 171 */ "xx_common_expr ::= IDENTIFIER ARROW IDENTIFIER",
 /* 172 */ "xx_common_expr ::= IDENTIFIER SBRACKET_OPEN IDENTIFIER SBRACKET_CLOSE",
 /* 173 */ "xx_common_expr ::= IDENTIFIER SBRACKET_OPEN INTEGER SBRACKET_CLOSE",
 /* 174 */ "xx_common_expr ::= IDENTIFIER SBRACKET_OPEN STRING SBRACKET_CLOSE",
 /* 175 */ "xx_common_expr ::= xx_common_expr ADD xx_common_expr",
 /* 176 */ "xx_common_expr ::= xx_common_expr SUB xx_common_expr",
 /* 177 */ "xx_common_expr ::= xx_common_expr CONCAT xx_common_expr",
 /* 178 */ "xx_common_expr ::= xx_common_expr AND xx_common_expr",
 /* 179 */ "xx_common_expr ::= xx_common_expr OR xx_common_expr",
 /* 180 */ "xx_common_expr ::= INSTANCEOF xx_common_expr",
 /* 181 */ "xx_common_expr ::= FETCH IDENTIFIER COMMA xx_isset_expr",
 /* 182 */ "xx_common_expr ::= TYPEOF xx_common_expr",
 /* 183 */ "xx_common_expr ::= IDENTIFIER",
 /* 184 */ "xx_common_expr ::= INTEGER",
 /* 185 */ "xx_common_expr ::= STRING",
 /* 186 */ "xx_common_expr ::= DOUBLE",
 /* 187 */ "xx_common_expr ::= NULL",
 /* 188 */ "xx_common_expr ::= TRUE",
 /* 189 */ "xx_common_expr ::= FALSE",
 /* 190 */ "xx_common_expr ::= SBRACKET_OPEN SBRACKET_CLOSE",
 /* 191 */ "xx_common_expr ::= SBRACKET_OPEN xx_array_list SBRACKET_CLOSE",
 /* 192 */ "xx_common_expr ::= NEW IDENTIFIER",
 /* 193 */ "xx_common_expr ::= NEW IDENTIFIER PARENTHESES_OPEN PARENTHESES_CLOSE",
 /* 194 */ "xx_common_expr ::= NEW IDENTIFIER PARENTHESES_OPEN xx_call_parameters PARENTHESES_CLOSE",
 /* 195 */ "xx_common_expr ::= IDENTIFIER PARENTHESES_OPEN xx_call_parameters PARENTHESES_CLOSE",
 /* 196 */ "xx_common_expr ::= IDENTIFIER PARENTHESES_OPEN PARENTHESES_CLOSE",
 /* 197 */ "xx_common_expr ::= xx_mcall_expr",
 /* 198 */ "xx_mcall_expr ::= IDENTIFIER ARROW IDENTIFIER PARENTHESES_OPEN xx_call_parameters PARENTHESES_CLOSE",
 /* 199 */ "xx_mcall_expr ::= IDENTIFIER ARROW IDENTIFIER PARENTHESES_OPEN PARENTHESES_CLOSE",
 /* 200 */ "xx_call_parameters ::= xx_call_parameters COMMA xx_call_parameter",
 /* 201 */ "xx_call_parameters ::= xx_call_parameter",
 /* 202 */ "xx_call_parameter ::= xx_common_expr",
 /* 203 */ "xx_array_list ::= xx_array_list COMMA xx_array_item",
 /* 204 */ "xx_array_list ::= xx_array_item",
 /* 205 */ "xx_array_item ::= xx_array_key COLON xx_array_value",
 /* 206 */ "xx_array_item ::= xx_array_value",
 /* 207 */ "xx_array_key ::= IDENTIFIER",
 /* 208 */ "xx_array_key ::= STRING",
 /* 209 */ "xx_array_key ::= INTEGER",
 /* 210 */ "xx_array_value ::= xx_literal_expr",
 /* 211 */ "xx_array_value ::= IDENTIFIER",
 /* 212 */ "xx_literal_expr ::= INTEGER",
 /* 213 */ "xx_literal_expr ::= STRING",
 /* 214 */ "xx_literal_expr ::= DOUBLE",
 /* 215 */ "xx_literal_expr ::= NULL",
 /* 216 */ "xx_literal_expr ::= FALSE",
 /* 217 */ "xx_literal_expr ::= TRUE",
 /* 218 */ "xx_isset_expr ::= IDENTIFIER SBRACKET_OPEN IDENTIFIER SBRACKET_CLOSE",
 /* 219 */ "xx_isset_expr ::= IDENTIFIER SBRACKET_OPEN INTEGER SBRACKET_CLOSE",
 /* 220 */ "xx_isset_expr ::= IDENTIFIER SBRACKET_OPEN STRING SBRACKET_CLOSE",
 /* 221 */ "xx_eval_expr ::= xx_common_expr",
 /* 222 */ "xx_comment ::= COMMENT",
};
#endif /* NDEBUG */

/*
** This function returns the symbolic name associated with a token
** value.
*/
const char *xx_TokenName(int tokenType){
#ifndef NDEBUG
  if( tokenType>0 && tokenType<(sizeof(yyTokenName)/sizeof(yyTokenName[0])) ){
    return yyTokenName[tokenType];
  }else{
    return "Unknown";
  }
#else
  return "";
#endif
}

/* 
** This function allocates a new parser.
** The only argument is a pointer to a function which works like
** malloc.
**
** Inputs:
** A pointer to the function used to allocate memory.
**
** Outputs:
** A pointer to a parser.  This pointer is used in subsequent calls
** to xx_ and xx_Free.
*/
void *xx_Alloc(void *(*mallocProc)(size_t)){
  yyParser *pParser;
  pParser = (yyParser*)(*mallocProc)( (size_t)sizeof(yyParser) );
  if( pParser ){
    pParser->yyidx = -1;
  }
  return pParser;
}

/* The following function deletes the value associated with a
** symbol.  The symbol can be either a terminal or nonterminal.
** "yymajor" is the symbol code, and "yypminor" is a pointer to
** the value.
*/
static void yy_destructor(YYCODETYPE yymajor, YYMINORTYPE *yypminor){
  switch( yymajor ){
    /* Here is inserted the actions which take place when a
    ** terminal or non-terminal is destroyed.  This can happen
    ** when the symbol is popped from the stack during a
    ** reduce or during error processing or when a parser is 
    ** being destroyed before it is finished parsing.
    **
    ** Note: during a reduce, the only symbols destroyed are those
    ** which appear on the RHS of the rule, but which are not used
    ** inside the C code.
    */
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
    case 37:
    case 38:
    case 39:
    case 40:
    case 41:
    case 42:
    case 43:
    case 44:
    case 45:
    case 46:
    case 47:
    case 48:
    case 49:
    case 50:
    case 51:
    case 52:
    case 53:
    case 54:
    case 55:
    case 56:
    case 57:
    case 58:
    case 59:
    case 60:
    case 61:
    case 62:
    case 63:
    case 64:
    case 65:
    case 66:
    case 67:
    case 68:
    case 69:
    case 70:
    case 71:
    case 72:
    case 73:
    case 74:
    case 75:
    case 76:
    case 77:
    case 78:
    case 79:
// 758 "parser.lemon"
{
	/*if ((yypminor->yy0)) {
		if ((yypminor->yy0)->free_flag) {
			efree((yypminor->yy0)->token);
		}
		efree((yypminor->yy0));
	}*/
}
// 1903 "parser.c"
      break;
    case 82:
// 771 "parser.lemon"
{ json_object_put((yypminor->yy25)); }
// 1908 "parser.c"
      break;
    default:  break;   /* If no destructor action specified: do nothing */
  }
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
**
** Return the major token number for the symbol popped.
*/
static int yy_pop_parser_stack(yyParser *pParser){
  YYCODETYPE yymajor;
  yyStackEntry *yytos = &pParser->yystack[pParser->yyidx];

  if( pParser->yyidx<0 ) return 0;
#ifndef NDEBUG
  if( yyTraceFILE && pParser->yyidx>=0 ){
    fprintf(yyTraceFILE,"%sPopping %s\n",
      yyTracePrompt,
      yyTokenName[yytos->major]);
  }
#endif
  yymajor = yytos->major;
  yy_destructor( yymajor, &yytos->minor);
  pParser->yyidx--;
  return yymajor;
}

/* 
** Deallocate and destroy a parser.  Destructors are all called for
** all stack elements before shutting the parser down.
**
** Inputs:
** <ul>
** <li>  A pointer to the parser.  This should be a pointer
**       obtained from xx_Alloc.
** <li>  A pointer to a function used to reclaim memory obtained
**       from malloc.
** </ul>
*/
void xx_Free(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
  yyParser *pParser = (yyParser*)p;
  if( pParser==0 ) return;
  while( pParser->yyidx>=0 ) yy_pop_parser_stack(pParser);
  (*freeProc)((void*)pParser);
}

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_shift_action(
  yyParser *pParser,        /* The parser */
  int iLookAhead            /* The look-ahead token */
){
  int i;
  int stateno = pParser->yystack[pParser->yyidx].stateno;
 
  /* if( pParser->yyidx<0 ) return YY_NO_ACTION;  */
  i = yy_shift_ofst[stateno];
  if( i==YY_SHIFT_USE_DFLT ){
    return yy_default[stateno];
  }
  if( iLookAhead==YYNOCODE ){
    return YY_NO_ACTION;
  }
  i += iLookAhead;
  if( i<0 || i>=YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead ){
#ifdef YYFALLBACK
    int iFallback;            /* Fallback token */
    if( iLookAhead<sizeof(yyFallback)/sizeof(yyFallback[0])
           && (iFallback = yyFallback[iLookAhead])!=0 ){
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE, "%sFALLBACK %s => %s\n",
           yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[iFallback]);
      }
#endif
      return yy_find_shift_action(pParser, iFallback);
    }
#endif
    return yy_default[stateno];
  }else{
    return yy_action[i];
  }
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_reduce_action(
  yyParser *pParser,        /* The parser */
  int iLookAhead            /* The look-ahead token */
){
  int i;
  int stateno = pParser->yystack[pParser->yyidx].stateno;
 
  i = yy_reduce_ofst[stateno];
  if( i==YY_REDUCE_USE_DFLT ){
    return yy_default[stateno];
  }
  if( iLookAhead==YYNOCODE ){
    return YY_NO_ACTION;
  }
  i += iLookAhead;
  if( i<0 || i>=YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead ){
    return yy_default[stateno];
  }else{
    return yy_action[i];
  }
}

/*
** Perform a shift action.
*/
static void yy_shift(
  yyParser *yypParser,          /* The parser to be shifted */
  int yyNewState,               /* The new state to shift in */
  int yyMajor,                  /* The major token to shift in */
  YYMINORTYPE *yypMinor         /* Pointer ot the minor token to shift in */
){
  yyStackEntry *yytos;
  yypParser->yyidx++;
  if( yypParser->yyidx>=YYSTACKDEPTH ){
     xx_ARG_FETCH;
     yypParser->yyidx--;
#ifndef NDEBUG
     if( yyTraceFILE ){
       fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
     }
#endif
     while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
     /* Here code is inserted which will execute if the parser
     ** stack every overflows */
     xx_ARG_STORE; /* Suppress warning about unused %extra_argument var */
     return;
  }
  yytos = &yypParser->yystack[yypParser->yyidx];
  yytos->stateno = yyNewState;
  yytos->major = yyMajor;
  yytos->minor = *yypMinor;
#ifndef NDEBUG
  if( yyTraceFILE && yypParser->yyidx>0 ){
    int i;
    fprintf(yyTraceFILE,"%sShift %d\n",yyTracePrompt,yyNewState);
    fprintf(yyTraceFILE,"%sStack:",yyTracePrompt);
    for(i=1; i<=yypParser->yyidx; i++)
      fprintf(yyTraceFILE," %s",yyTokenName[yypParser->yystack[i].major]);
    fprintf(yyTraceFILE,"\n");
  }
#endif
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static struct {
  YYCODETYPE lhs;         /* Symbol on the left-hand side of the rule */
  unsigned char nrhs;     /* Number of right-hand side symbols in the rule */
} yyRuleInfo[] = {
  { 81, 1 },
  { 82, 1 },
  { 83, 2 },
  { 83, 1 },
  { 84, 1 },
  { 84, 1 },
  { 84, 1 },
  { 85, 3 },
  { 86, 5 },
  { 86, 7 },
  { 86, 6 },
  { 86, 8 },
  { 86, 4 },
  { 86, 6 },
  { 86, 5 },
  { 86, 7 },
  { 88, 1 },
  { 88, 1 },
  { 88, 1 },
  { 88, 2 },
  { 88, 2 },
  { 88, 2 },
  { 88, 2 },
  { 88, 3 },
  { 88, 3 },
  { 89, 2 },
  { 89, 1 },
  { 92, 4 },
  { 92, 3 },
  { 92, 6 },
  { 92, 5 },
  { 93, 2 },
  { 93, 1 },
  { 95, 1 },
  { 95, 1 },
  { 95, 1 },
  { 95, 1 },
  { 90, 2 },
  { 90, 1 },
  { 91, 2 },
  { 91, 1 },
  { 96, 6 },
  { 96, 5 },
  { 97, 7 },
  { 97, 8 },
  { 97, 8 },
  { 97, 9 },
  { 97, 8 },
  { 97, 9 },
  { 97, 9 },
  { 97, 10 },
  { 98, 1 },
  { 101, 1 },
  { 101, 1 },
  { 101, 1 },
  { 101, 1 },
  { 101, 2 },
  { 101, 2 },
  { 101, 2 },
  { 101, 2 },
  { 101, 2 },
  { 101, 2 },
  { 101, 2 },
  { 101, 2 },
  { 101, 2 },
  { 101, 2 },
  { 99, 3 },
  { 99, 1 },
  { 102, 1 },
  { 102, 2 },
  { 102, 2 },
  { 102, 3 },
  { 102, 4 },
  { 102, 4 },
  { 104, 3 },
  { 103, 1 },
  { 103, 1 },
  { 103, 1 },
  { 103, 1 },
  { 103, 1 },
  { 103, 1 },
  { 100, 2 },
  { 100, 1 },
  { 105, 1 },
  { 105, 1 },
  { 105, 1 },
  { 105, 1 },
  { 105, 1 },
  { 105, 1 },
  { 105, 1 },
  { 105, 1 },
  { 105, 1 },
  { 105, 1 },
  { 105, 1 },
  { 105, 1 },
  { 105, 1 },
  { 105, 1 },
  { 105, 1 },
  { 115, 2 },
  { 116, 2 },
  { 107, 4 },
  { 107, 7 },
  { 107, 5 },
  { 107, 9 },
  { 107, 8 },
  { 107, 8 },
  { 118, 4 },
  { 118, 5 },
  { 121, 2 },
  { 121, 1 },
  { 122, 4 },
  { 122, 3 },
  { 108, 3 },
  { 108, 4 },
  { 117, 4 },
  { 117, 5 },
  { 119, 7 },
  { 119, 8 },
  { 119, 9 },
  { 119, 10 },
  { 106, 3 },
  { 124, 3 },
  { 124, 1 },
  { 126, 1 },
  { 126, 1 },
  { 126, 1 },
  { 126, 1 },
  { 126, 1 },
  { 126, 1 },
  { 125, 3 },
  { 125, 5 },
  { 125, 7 },
  { 125, 8 },
  { 125, 5 },
  { 125, 6 },
  { 125, 2 },
  { 125, 2 },
  { 128, 1 },
  { 109, 3 },
  { 129, 3 },
  { 129, 1 },
  { 130, 1 },
  { 111, 2 },
  { 110, 3 },
  { 112, 5 },
  { 112, 6 },
  { 112, 6 },
  { 112, 6 },
  { 113, 3 },
  { 113, 7 },
  { 114, 3 },
  { 114, 3 },
  { 114, 3 },
  { 114, 3 },
  { 114, 3 },
  { 133, 3 },
  { 133, 1 },
  { 134, 1 },
  { 134, 3 },
  { 127, 1 },
  { 123, 2 },
  { 123, 2 },
  { 123, 3 },
  { 123, 3 },
  { 123, 3 },
  { 123, 3 },
  { 123, 3 },
  { 123, 3 },
  { 123, 3 },
  { 123, 3 },
  { 123, 3 },
  { 123, 3 },
  { 123, 4 },
  { 123, 4 },
  { 123, 4 },
  { 123, 3 },
  { 123, 3 },
  { 123, 3 },
  { 123, 3 },
  { 123, 3 },
  { 123, 2 },
  { 123, 4 },
  { 123, 2 },
  { 123, 1 },
  { 123, 1 },
  { 123, 1 },
  { 123, 1 },
  { 123, 1 },
  { 123, 1 },
  { 123, 1 },
  { 123, 2 },
  { 123, 3 },
  { 123, 2 },
  { 123, 4 },
  { 123, 5 },
  { 123, 4 },
  { 123, 3 },
  { 123, 1 },
  { 131, 6 },
  { 131, 5 },
  { 132, 3 },
  { 132, 1 },
  { 137, 1 },
  { 136, 3 },
  { 136, 1 },
  { 138, 3 },
  { 138, 1 },
  { 139, 1 },
  { 139, 1 },
  { 139, 1 },
  { 140, 1 },
  { 140, 1 },
  { 94, 1 },
  { 94, 1 },
  { 94, 1 },
  { 94, 1 },
  { 94, 1 },
  { 94, 1 },
  { 135, 4 },
  { 135, 4 },
  { 135, 4 },
  { 120, 1 },
  { 87, 1 },
};

static void yy_accept(yyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void yy_reduce(
  yyParser *yypParser,         /* The parser */
  int yyruleno                 /* Number of the rule by which to reduce */
){
  int yygoto;                     /* The next state */
  int yyact;                      /* The next action */
  YYMINORTYPE yygotominor;        /* The LHS of the rule reduced */
  yyStackEntry *yymsp;            /* The top of the parser's stack */
  int yysize;                     /* Amount to pop the stack */
  xx_ARG_FETCH;
  yymsp = &yypParser->yystack[yypParser->yyidx];
#ifndef NDEBUG
  if( yyTraceFILE && yyruleno>=0 
        && yyruleno<sizeof(yyRuleName)/sizeof(yyRuleName[0]) ){
    fprintf(yyTraceFILE, "%sReduce [%s].\n", yyTracePrompt,
      yyRuleName[yyruleno]);
  }
#endif /* NDEBUG */

  switch( yyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  // <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  // <lineno> <thisfile>
  **     break;
  */
      case 0:
// 767 "parser.lemon"
{
	status->ret = yymsp[0].minor.yy25;
}
// 2348 "parser.c"
        break;
      case 1:
      case 4:
      case 5:
      case 6:
      case 83:
      case 84:
      case 85:
      case 86:
      case 87:
      case 88:
      case 89:
      case 90:
      case 91:
      case 92:
      case 93:
      case 94:
      case 95:
      case 96:
      case 97:
      case 137:
      case 159:
      case 197:
      case 202:
      case 210:
      case 221:
// 773 "parser.lemon"
{
	yygotominor.yy25 = yymsp[0].minor.yy25;
}
// 2379 "parser.c"
        break;
      case 2:
      case 25:
      case 31:
      case 37:
      case 39:
      case 81:
      case 108:
// 777 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_list(yymsp[-1].minor.yy25, yymsp[0].minor.yy25);
}
// 2392 "parser.c"
        break;
      case 3:
      case 26:
      case 32:
      case 38:
      case 40:
      case 51:
      case 67:
      case 82:
      case 109:
      case 122:
      case 140:
      case 156:
      case 201:
      case 204:
// 781 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_list(NULL, yymsp[0].minor.yy25);
}
// 2412 "parser.c"
        break;
      case 7:
// 797 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_namespace(yymsp[-1].minor.yy0, status->scanner_state);
  yy_destructor(25,&yymsp[-2].minor);
  yy_destructor(27,&yymsp[0].minor);
}
// 2421 "parser.c"
        break;
      case 8:
// 801 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_class(yymsp[-3].minor.yy0, yymsp[-1].minor.yy25, 0, NULL, status->scanner_state);
  yy_destructor(28,&yymsp[-4].minor);
  yy_destructor(29,&yymsp[-2].minor);
  yy_destructor(30,&yymsp[0].minor);
}
// 2431 "parser.c"
        break;
      case 9:
// 805 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_class(yymsp[-5].minor.yy0, yymsp[-1].minor.yy25, 0, yymsp[-3].minor.yy0, status->scanner_state);
  yy_destructor(28,&yymsp[-6].minor);
  yy_destructor(31,&yymsp[-4].minor);
  yy_destructor(29,&yymsp[-2].minor);
  yy_destructor(30,&yymsp[0].minor);
}
// 2442 "parser.c"
        break;
      case 10:
// 809 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_class(yymsp[-3].minor.yy0, yymsp[-1].minor.yy25, 1, NULL, status->scanner_state);
  yy_destructor(32,&yymsp[-5].minor);
  yy_destructor(28,&yymsp[-4].minor);
  yy_destructor(29,&yymsp[-2].minor);
  yy_destructor(30,&yymsp[0].minor);
}
// 2453 "parser.c"
        break;
      case 11:
// 813 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_class(yymsp[-5].minor.yy0, yymsp[-1].minor.yy25, 1, yymsp[-3].minor.yy0, status->scanner_state);
  yy_destructor(32,&yymsp[-7].minor);
  yy_destructor(28,&yymsp[-6].minor);
  yy_destructor(31,&yymsp[-4].minor);
  yy_destructor(29,&yymsp[-2].minor);
  yy_destructor(30,&yymsp[0].minor);
}
// 2465 "parser.c"
        break;
      case 12:
// 817 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_class(yymsp[-2].minor.yy0, NULL, 0, NULL, status->scanner_state);
  yy_destructor(28,&yymsp[-3].minor);
  yy_destructor(29,&yymsp[-1].minor);
  yy_destructor(30,&yymsp[0].minor);
}
// 2475 "parser.c"
        break;
      case 13:
// 821 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_class(yymsp[-4].minor.yy0, NULL, 0, yymsp[-2].minor.yy0, status->scanner_state);
  yy_destructor(28,&yymsp[-5].minor);
  yy_destructor(31,&yymsp[-3].minor);
  yy_destructor(29,&yymsp[-1].minor);
  yy_destructor(30,&yymsp[0].minor);
}
// 2486 "parser.c"
        break;
      case 14:
// 825 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_class(yymsp[-2].minor.yy0, NULL, 1, NULL, status->scanner_state);
  yy_destructor(32,&yymsp[-4].minor);
  yy_destructor(28,&yymsp[-3].minor);
  yy_destructor(29,&yymsp[-1].minor);
  yy_destructor(30,&yymsp[0].minor);
}
// 2497 "parser.c"
        break;
      case 15:
// 829 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_class(yymsp[-4].minor.yy0, NULL, 1, yymsp[-2].minor.yy0, status->scanner_state);
  yy_destructor(32,&yymsp[-6].minor);
  yy_destructor(28,&yymsp[-5].minor);
  yy_destructor(31,&yymsp[-3].minor);
  yy_destructor(29,&yymsp[-1].minor);
  yy_destructor(30,&yymsp[0].minor);
}
// 2509 "parser.c"
        break;
      case 16:
// 833 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_class_definition(yymsp[0].minor.yy25, NULL, NULL, status->scanner_state);
}
// 2516 "parser.c"
        break;
      case 17:
// 837 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_class_definition(NULL, NULL, yymsp[0].minor.yy25, status->scanner_state);
}
// 2523 "parser.c"
        break;
      case 18:
// 841 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_class_definition(NULL, yymsp[0].minor.yy25, NULL, status->scanner_state);
}
// 2530 "parser.c"
        break;
      case 19:
// 845 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_class_definition(yymsp[-1].minor.yy25, yymsp[0].minor.yy25, NULL, status->scanner_state);
}
// 2537 "parser.c"
        break;
      case 20:
// 849 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_class_definition(yymsp[-1].minor.yy25, NULL, yymsp[0].minor.yy25, status->scanner_state);
}
// 2544 "parser.c"
        break;
      case 21:
// 853 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_class_definition(yymsp[0].minor.yy25, NULL, yymsp[-1].minor.yy25, status->scanner_state);
}
// 2551 "parser.c"
        break;
      case 22:
// 857 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_class_definition(NULL, yymsp[0].minor.yy25, yymsp[-1].minor.yy25, status->scanner_state);
}
// 2558 "parser.c"
        break;
      case 23:
// 861 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_class_definition(yymsp[-2].minor.yy25, yymsp[0].minor.yy25, yymsp[-1].minor.yy25, status->scanner_state);
}
// 2565 "parser.c"
        break;
      case 24:
// 865 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_class_definition(yymsp[-1].minor.yy25, yymsp[0].minor.yy25, yymsp[-2].minor.yy25, status->scanner_state);
}
// 2572 "parser.c"
        break;
      case 27:
// 877 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_class_property(yymsp[-2].minor.yy25, yymsp[-1].minor.yy0, NULL, yymsp[-3].minor.yy0, status->scanner_state);
  yy_destructor(27,&yymsp[0].minor);
}
// 2580 "parser.c"
        break;
      case 28:
// 881 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_class_property(yymsp[-2].minor.yy25, yymsp[-1].minor.yy0, NULL, NULL, status->scanner_state);
  yy_destructor(27,&yymsp[0].minor);
}
// 2588 "parser.c"
        break;
      case 29:
// 885 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_class_property(yymsp[-4].minor.yy25, yymsp[-3].minor.yy0, yymsp[-1].minor.yy25, yymsp[-5].minor.yy0, status->scanner_state);
  yy_destructor(34,&yymsp[-2].minor);
  yy_destructor(27,&yymsp[0].minor);
}
// 2597 "parser.c"
        break;
      case 30:
// 889 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_class_property(yymsp[-4].minor.yy25, yymsp[-3].minor.yy0, yymsp[-1].minor.yy25, NULL, status->scanner_state);
  yy_destructor(34,&yymsp[-2].minor);
  yy_destructor(27,&yymsp[0].minor);
}
// 2606 "parser.c"
        break;
      case 33:
// 901 "parser.lemon"
{
	yygotominor.yy25 = json_object_new_string("public");
  yy_destructor(1,&yymsp[0].minor);
}
// 2614 "parser.c"
        break;
      case 34:
// 905 "parser.lemon"
{
	yygotominor.yy25 = json_object_new_string("protected");
  yy_destructor(2,&yymsp[0].minor);
}
// 2622 "parser.c"
        break;
      case 35:
// 909 "parser.lemon"
{
	yygotominor.yy25 = json_object_new_string("static");
  yy_destructor(3,&yymsp[0].minor);
}
// 2630 "parser.c"
        break;
      case 36:
// 913 "parser.lemon"
{
	yygotominor.yy25 = json_object_new_string("private");
  yy_destructor(4,&yymsp[0].minor);
}
// 2638 "parser.c"
        break;
      case 41:
// 933 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_class_const(yymsp[-3].minor.yy0, yymsp[-1].minor.yy25, yymsp[-5].minor.yy0, status->scanner_state);
  yy_destructor(35,&yymsp[-4].minor);
  yy_destructor(34,&yymsp[-2].minor);
  yy_destructor(27,&yymsp[0].minor);
}
// 2648 "parser.c"
        break;
      case 42:
// 937 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_class_const(yymsp[-3].minor.yy0, yymsp[-1].minor.yy25, NULL, status->scanner_state);
  yy_destructor(35,&yymsp[-4].minor);
  yy_destructor(34,&yymsp[-2].minor);
  yy_destructor(27,&yymsp[0].minor);
}
// 2658 "parser.c"
        break;
      case 43:
// 941 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_class_method(yymsp[-6].minor.yy25, yymsp[-4].minor.yy0, NULL, NULL, NULL, status->scanner_state);
  yy_destructor(36,&yymsp[-5].minor);
  yy_destructor(37,&yymsp[-3].minor);
  yy_destructor(38,&yymsp[-2].minor);
  yy_destructor(29,&yymsp[-1].minor);
  yy_destructor(30,&yymsp[0].minor);
}
// 2670 "parser.c"
        break;
      case 44:
// 945 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_class_method(yymsp[-7].minor.yy25, yymsp[-5].minor.yy0, yymsp[-3].minor.yy25, NULL, NULL, status->scanner_state);
  yy_destructor(36,&yymsp[-6].minor);
  yy_destructor(37,&yymsp[-4].minor);
  yy_destructor(38,&yymsp[-2].minor);
  yy_destructor(29,&yymsp[-1].minor);
  yy_destructor(30,&yymsp[0].minor);
}
// 2682 "parser.c"
        break;
      case 45:
// 949 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_class_method(yymsp[-7].minor.yy25, yymsp[-5].minor.yy0, NULL, yymsp[-1].minor.yy25, NULL, status->scanner_state);
  yy_destructor(36,&yymsp[-6].minor);
  yy_destructor(37,&yymsp[-4].minor);
  yy_destructor(38,&yymsp[-3].minor);
  yy_destructor(29,&yymsp[-2].minor);
  yy_destructor(30,&yymsp[0].minor);
}
// 2694 "parser.c"
        break;
      case 46:
// 953 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_class_method(yymsp[-8].minor.yy25, yymsp[-6].minor.yy0, yymsp[-4].minor.yy25, yymsp[-1].minor.yy25, NULL, status->scanner_state);
  yy_destructor(36,&yymsp[-7].minor);
  yy_destructor(37,&yymsp[-5].minor);
  yy_destructor(38,&yymsp[-3].minor);
  yy_destructor(29,&yymsp[-2].minor);
  yy_destructor(30,&yymsp[0].minor);
}
// 2706 "parser.c"
        break;
      case 47:
// 957 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_class_method(yymsp[-6].minor.yy25, yymsp[-4].minor.yy0, NULL, NULL, yymsp[-7].minor.yy0, status->scanner_state);
  yy_destructor(36,&yymsp[-5].minor);
  yy_destructor(37,&yymsp[-3].minor);
  yy_destructor(38,&yymsp[-2].minor);
  yy_destructor(29,&yymsp[-1].minor);
  yy_destructor(30,&yymsp[0].minor);
}
// 2718 "parser.c"
        break;
      case 48:
// 961 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_class_method(yymsp[-7].minor.yy25, yymsp[-5].minor.yy0, yymsp[-3].minor.yy25, NULL, yymsp[-8].minor.yy0, status->scanner_state);
  yy_destructor(36,&yymsp[-6].minor);
  yy_destructor(37,&yymsp[-4].minor);
  yy_destructor(38,&yymsp[-2].minor);
  yy_destructor(29,&yymsp[-1].minor);
  yy_destructor(30,&yymsp[0].minor);
}
// 2730 "parser.c"
        break;
      case 49:
// 965 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_class_method(yymsp[-7].minor.yy25, yymsp[-5].minor.yy0, NULL, yymsp[-1].minor.yy25, yymsp[-8].minor.yy0, status->scanner_state);
  yy_destructor(36,&yymsp[-6].minor);
  yy_destructor(37,&yymsp[-4].minor);
  yy_destructor(38,&yymsp[-3].minor);
  yy_destructor(29,&yymsp[-2].minor);
  yy_destructor(30,&yymsp[0].minor);
}
// 2742 "parser.c"
        break;
      case 50:
// 969 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_class_method(yymsp[-8].minor.yy25, yymsp[-6].minor.yy0, yymsp[-4].minor.yy25, yymsp[-1].minor.yy25, yymsp[-9].minor.yy0, status->scanner_state);
  yy_destructor(36,&yymsp[-7].minor);
  yy_destructor(37,&yymsp[-5].minor);
  yy_destructor(38,&yymsp[-3].minor);
  yy_destructor(29,&yymsp[-2].minor);
  yy_destructor(30,&yymsp[0].minor);
}
// 2754 "parser.c"
        break;
      case 52:
// 981 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_list(NULL, json_object_new_string("public"));
  yy_destructor(1,&yymsp[0].minor);
}
// 2762 "parser.c"
        break;
      case 53:
// 985 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_list(NULL, json_object_new_string("protected"));
  yy_destructor(2,&yymsp[0].minor);
}
// 2770 "parser.c"
        break;
      case 54:
// 989 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_list(NULL, json_object_new_string("static"));
  yy_destructor(3,&yymsp[0].minor);
}
// 2778 "parser.c"
        break;
      case 55:
// 993 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_list(NULL, json_object_new_string("private"));
  yy_destructor(4,&yymsp[0].minor);
}
// 2786 "parser.c"
        break;
      case 56:
// 997 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_list(json_object_new_string("static"), json_object_new_string("public"));
  yy_destructor(3,&yymsp[-1].minor);
  yy_destructor(1,&yymsp[0].minor);
}
// 2795 "parser.c"
        break;
      case 57:
// 1001 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_list(json_object_new_string("static"), json_object_new_string("public"));
  yy_destructor(1,&yymsp[-1].minor);
  yy_destructor(3,&yymsp[0].minor);
}
// 2804 "parser.c"
        break;
      case 58:
// 1005 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_list(json_object_new_string("static"), json_object_new_string("protected"));
  yy_destructor(3,&yymsp[-1].minor);
  yy_destructor(2,&yymsp[0].minor);
}
// 2813 "parser.c"
        break;
      case 59:
// 1009 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_list(json_object_new_string("static"), json_object_new_string("protected"));
  yy_destructor(2,&yymsp[-1].minor);
  yy_destructor(3,&yymsp[0].minor);
}
// 2822 "parser.c"
        break;
      case 60:
// 1013 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_list(json_object_new_string("static"), json_object_new_string("private"));
  yy_destructor(3,&yymsp[-1].minor);
  yy_destructor(4,&yymsp[0].minor);
}
// 2831 "parser.c"
        break;
      case 61:
// 1017 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_list(json_object_new_string("static"), json_object_new_string("private"));
  yy_destructor(4,&yymsp[-1].minor);
  yy_destructor(3,&yymsp[0].minor);
}
// 2840 "parser.c"
        break;
      case 62:
// 1021 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_list(json_object_new_string("inline"), json_object_new_string("protected"));
  yy_destructor(39,&yymsp[-1].minor);
  yy_destructor(2,&yymsp[0].minor);
}
// 2849 "parser.c"
        break;
      case 63:
// 1025 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_list(json_object_new_string("inline"), json_object_new_string("protected"));
  yy_destructor(2,&yymsp[-1].minor);
  yy_destructor(39,&yymsp[0].minor);
}
// 2858 "parser.c"
        break;
      case 64:
// 1029 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_list(json_object_new_string("inline"), json_object_new_string("private"));
  yy_destructor(39,&yymsp[-1].minor);
  yy_destructor(4,&yymsp[0].minor);
}
// 2867 "parser.c"
        break;
      case 65:
// 1033 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_list(json_object_new_string("inline"), json_object_new_string("private"));
  yy_destructor(4,&yymsp[-1].minor);
  yy_destructor(39,&yymsp[0].minor);
}
// 2876 "parser.c"
        break;
      case 66:
      case 121:
      case 139:
      case 155:
      case 200:
      case 203:
// 1037 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_list(yymsp[-2].minor.yy25, yymsp[0].minor.yy25);
  yy_destructor(5,&yymsp[-1].minor);
}
// 2889 "parser.c"
        break;
      case 68:
// 1045 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_parameter(NULL, NULL, yymsp[0].minor.yy0, NULL, status->scanner_state);
}
// 2896 "parser.c"
        break;
      case 69:
// 1049 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_parameter(yymsp[-1].minor.yy25, NULL, yymsp[0].minor.yy0, NULL, status->scanner_state);
}
// 2903 "parser.c"
        break;
      case 70:
// 1053 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_parameter(NULL, yymsp[-1].minor.yy25, yymsp[0].minor.yy0, NULL, status->scanner_state);
}
// 2910 "parser.c"
        break;
      case 71:
// 1057 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_parameter(NULL, NULL, yymsp[-2].minor.yy0, yymsp[0].minor.yy25, status->scanner_state);
  yy_destructor(34,&yymsp[-1].minor);
}
// 2918 "parser.c"
        break;
      case 72:
// 1061 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_parameter(yymsp[-3].minor.yy25, NULL, yymsp[-2].minor.yy0, yymsp[0].minor.yy25, status->scanner_state);
  yy_destructor(34,&yymsp[-1].minor);
}
// 2926 "parser.c"
        break;
      case 73:
// 1065 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_parameter(NULL, yymsp[-3].minor.yy25, yymsp[-2].minor.yy0, yymsp[0].minor.yy25, status->scanner_state);
  yy_destructor(34,&yymsp[-1].minor);
}
// 2934 "parser.c"
        break;
      case 74:
// 1069 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_literal(XX_T_IDENTIFIER, yymsp[-1].minor.yy0, status->scanner_state);
  yy_destructor(10,&yymsp[-2].minor);
  yy_destructor(11,&yymsp[0].minor);
}
// 2943 "parser.c"
        break;
      case 75:
// 1073 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_type(XX_TYPE_INTEGER);
  yy_destructor(40,&yymsp[0].minor);
}
// 2951 "parser.c"
        break;
      case 76:
// 1077 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_type(XX_TYPE_DOUBLE);
  yy_destructor(41,&yymsp[0].minor);
}
// 2959 "parser.c"
        break;
      case 77:
// 1081 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_type(XX_TYPE_BOOL);
  yy_destructor(42,&yymsp[0].minor);
}
// 2967 "parser.c"
        break;
      case 78:
// 1085 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_type(XX_TYPE_STRING);
  yy_destructor(43,&yymsp[0].minor);
}
// 2975 "parser.c"
        break;
      case 79:
// 1089 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_type(XX_TYPE_ARRAY);
  yy_destructor(44,&yymsp[0].minor);
}
// 2983 "parser.c"
        break;
      case 80:
// 1093 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_type(XX_TYPE_VAR);
  yy_destructor(45,&yymsp[0].minor);
}
// 2991 "parser.c"
        break;
      case 98:
// 1165 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_break_statement(status->scanner_state);
  yy_destructor(46,&yymsp[-1].minor);
  yy_destructor(27,&yymsp[0].minor);
}
// 3000 "parser.c"
        break;
      case 99:
// 1169 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_continue_statement(status->scanner_state);
  yy_destructor(47,&yymsp[-1].minor);
  yy_destructor(27,&yymsp[0].minor);
}
// 3009 "parser.c"
        break;
      case 100:
// 1173 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_if_statement(yymsp[-2].minor.yy25, NULL, NULL, status->scanner_state);
  yy_destructor(48,&yymsp[-3].minor);
  yy_destructor(29,&yymsp[-1].minor);
  yy_destructor(30,&yymsp[0].minor);
}
// 3019 "parser.c"
        break;
      case 101:
// 1177 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_if_statement(yymsp[-5].minor.yy25, NULL, NULL, status->scanner_state);
  yy_destructor(48,&yymsp[-6].minor);
  yy_destructor(29,&yymsp[-4].minor);
  yy_destructor(30,&yymsp[-3].minor);
  yy_destructor(49,&yymsp[-2].minor);
  yy_destructor(29,&yymsp[-1].minor);
  yy_destructor(30,&yymsp[0].minor);
}
// 3032 "parser.c"
        break;
      case 102:
// 1181 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_if_statement(yymsp[-3].minor.yy25, yymsp[-1].minor.yy25, NULL, status->scanner_state);
  yy_destructor(48,&yymsp[-4].minor);
  yy_destructor(29,&yymsp[-2].minor);
  yy_destructor(30,&yymsp[0].minor);
}
// 3042 "parser.c"
        break;
      case 103:
// 1185 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_if_statement(yymsp[-7].minor.yy25, yymsp[-5].minor.yy25, yymsp[-1].minor.yy25, status->scanner_state);
  yy_destructor(48,&yymsp[-8].minor);
  yy_destructor(29,&yymsp[-6].minor);
  yy_destructor(30,&yymsp[-4].minor);
  yy_destructor(49,&yymsp[-3].minor);
  yy_destructor(29,&yymsp[-2].minor);
  yy_destructor(30,&yymsp[0].minor);
}
// 3055 "parser.c"
        break;
      case 104:
// 1189 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_if_statement(yymsp[-6].minor.yy25, yymsp[-4].minor.yy25, NULL, status->scanner_state);
  yy_destructor(48,&yymsp[-7].minor);
  yy_destructor(29,&yymsp[-5].minor);
  yy_destructor(30,&yymsp[-3].minor);
  yy_destructor(49,&yymsp[-2].minor);
  yy_destructor(29,&yymsp[-1].minor);
  yy_destructor(30,&yymsp[0].minor);
}
// 3068 "parser.c"
        break;
      case 105:
// 1193 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_if_statement(yymsp[-6].minor.yy25, NULL, yymsp[-1].minor.yy25, status->scanner_state);
  yy_destructor(48,&yymsp[-7].minor);
  yy_destructor(29,&yymsp[-5].minor);
  yy_destructor(30,&yymsp[-4].minor);
  yy_destructor(49,&yymsp[-3].minor);
  yy_destructor(29,&yymsp[-2].minor);
  yy_destructor(30,&yymsp[0].minor);
}
// 3081 "parser.c"
        break;
      case 106:
// 1197 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_switch_statement(yymsp[-2].minor.yy25, NULL, status->scanner_state);
  yy_destructor(50,&yymsp[-3].minor);
  yy_destructor(29,&yymsp[-1].minor);
  yy_destructor(30,&yymsp[0].minor);
}
// 3091 "parser.c"
        break;
      case 107:
// 1201 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_switch_statement(yymsp[-3].minor.yy25, yymsp[-1].minor.yy25, status->scanner_state);
  yy_destructor(50,&yymsp[-4].minor);
  yy_destructor(29,&yymsp[-2].minor);
  yy_destructor(30,&yymsp[0].minor);
}
// 3101 "parser.c"
        break;
      case 110:
// 1213 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_case_clause(yymsp[-2].minor.yy25, yymsp[0].minor.yy25, status->scanner_state);
  yy_destructor(51,&yymsp[-3].minor);
  yy_destructor(52,&yymsp[-1].minor);
}
// 3110 "parser.c"
        break;
      case 111:
// 1217 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_case_clause(NULL, yymsp[0].minor.yy25, status->scanner_state);
  yy_destructor(53,&yymsp[-2].minor);
  yy_destructor(52,&yymsp[-1].minor);
}
// 3119 "parser.c"
        break;
      case 112:
// 1221 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_loop_statement(NULL, status->scanner_state);
  yy_destructor(54,&yymsp[-2].minor);
  yy_destructor(29,&yymsp[-1].minor);
  yy_destructor(30,&yymsp[0].minor);
}
// 3129 "parser.c"
        break;
      case 113:
// 1225 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_loop_statement(yymsp[-1].minor.yy25, status->scanner_state);
  yy_destructor(54,&yymsp[-3].minor);
  yy_destructor(29,&yymsp[-2].minor);
  yy_destructor(30,&yymsp[0].minor);
}
// 3139 "parser.c"
        break;
      case 114:
// 1229 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_while_statement(yymsp[-2].minor.yy25, NULL, status->scanner_state);
  yy_destructor(55,&yymsp[-3].minor);
  yy_destructor(29,&yymsp[-1].minor);
  yy_destructor(30,&yymsp[0].minor);
}
// 3149 "parser.c"
        break;
      case 115:
// 1233 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_while_statement(yymsp[-3].minor.yy25, yymsp[-1].minor.yy25, status->scanner_state);
  yy_destructor(55,&yymsp[-4].minor);
  yy_destructor(29,&yymsp[-2].minor);
  yy_destructor(30,&yymsp[0].minor);
}
// 3159 "parser.c"
        break;
      case 116:
// 1237 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_for_statement(yymsp[-3].minor.yy25, NULL, yymsp[-5].minor.yy0, 0, yymsp[-1].minor.yy25, status->scanner_state);
  yy_destructor(56,&yymsp[-6].minor);
  yy_destructor(57,&yymsp[-4].minor);
  yy_destructor(29,&yymsp[-2].minor);
  yy_destructor(30,&yymsp[0].minor);
}
// 3170 "parser.c"
        break;
      case 117:
// 1241 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_for_statement(yymsp[-3].minor.yy25, NULL, yymsp[-6].minor.yy0, 1, yymsp[-1].minor.yy25, status->scanner_state);
  yy_destructor(56,&yymsp[-7].minor);
  yy_destructor(57,&yymsp[-5].minor);
  yy_destructor(58,&yymsp[-4].minor);
  yy_destructor(29,&yymsp[-2].minor);
  yy_destructor(30,&yymsp[0].minor);
}
// 3182 "parser.c"
        break;
      case 118:
// 1245 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_for_statement(yymsp[-3].minor.yy25, yymsp[-7].minor.yy0, yymsp[-5].minor.yy0, 0, yymsp[-1].minor.yy25, status->scanner_state);
  yy_destructor(56,&yymsp[-8].minor);
  yy_destructor(5,&yymsp[-6].minor);
  yy_destructor(57,&yymsp[-4].minor);
  yy_destructor(29,&yymsp[-2].minor);
  yy_destructor(30,&yymsp[0].minor);
}
// 3194 "parser.c"
        break;
      case 119:
// 1249 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_for_statement(yymsp[-3].minor.yy25, yymsp[-8].minor.yy0, yymsp[-6].minor.yy0, 1, yymsp[-1].minor.yy25, status->scanner_state);
  yy_destructor(56,&yymsp[-9].minor);
  yy_destructor(5,&yymsp[-7].minor);
  yy_destructor(57,&yymsp[-5].minor);
  yy_destructor(58,&yymsp[-4].minor);
  yy_destructor(29,&yymsp[-2].minor);
  yy_destructor(30,&yymsp[0].minor);
}
// 3207 "parser.c"
        break;
      case 120:
// 1253 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_let_statement(yymsp[-1].minor.yy25, status->scanner_state);
  yy_destructor(59,&yymsp[-2].minor);
  yy_destructor(27,&yymsp[0].minor);
}
// 3216 "parser.c"
        break;
      case 123:
// 1266 "parser.lemon"
{
	yygotominor.yy25 = json_object_new_string("assign");
  yy_destructor(34,&yymsp[0].minor);
}
// 3224 "parser.c"
        break;
      case 124:
// 1271 "parser.lemon"
{
	yygotominor.yy25 = json_object_new_string("add-assign");
  yy_destructor(60,&yymsp[0].minor);
}
// 3232 "parser.c"
        break;
      case 125:
// 1276 "parser.lemon"
{
	yygotominor.yy25 = json_object_new_string("sub-assign");
  yy_destructor(61,&yymsp[0].minor);
}
// 3240 "parser.c"
        break;
      case 126:
// 1280 "parser.lemon"
{
	yygotominor.yy25 = json_object_new_string("mult-assign");
  yy_destructor(62,&yymsp[0].minor);
}
// 3248 "parser.c"
        break;
      case 127:
// 1284 "parser.lemon"
{
	yygotominor.yy25 = json_object_new_string("div-assign");
  yy_destructor(63,&yymsp[0].minor);
}
// 3256 "parser.c"
        break;
      case 128:
// 1288 "parser.lemon"
{
	yygotominor.yy25 = json_object_new_string("concat-assign");
  yy_destructor(64,&yymsp[0].minor);
}
// 3264 "parser.c"
        break;
      case 129:
// 1293 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_let_assignment("variable", yymsp[-1].minor.yy25, yymsp[-2].minor.yy0, NULL, NULL, yymsp[0].minor.yy25, status->scanner_state);
}
// 3271 "parser.c"
        break;
      case 130:
// 1298 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_let_assignment("object-property", yymsp[-1].minor.yy25, yymsp[-4].minor.yy0, yymsp[-2].minor.yy0, NULL, yymsp[0].minor.yy25, status->scanner_state);
  yy_destructor(65,&yymsp[-3].minor);
}
// 3279 "parser.c"
        break;
      case 131:
// 1303 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_let_assignment("object-property-append", yymsp[-1].minor.yy25, yymsp[-6].minor.yy0, yymsp[-4].minor.yy0, NULL, yymsp[0].minor.yy25, status->scanner_state);
  yy_destructor(65,&yymsp[-5].minor);
  yy_destructor(66,&yymsp[-3].minor);
  yy_destructor(67,&yymsp[-2].minor);
}
// 3289 "parser.c"
        break;
      case 132:
// 1308 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_let_assignment("object-property-array-index", yymsp[-1].minor.yy25, yymsp[-7].minor.yy0, yymsp[-5].minor.yy0, yymsp[-3].minor.yy25, yymsp[0].minor.yy25, status->scanner_state);
  yy_destructor(65,&yymsp[-6].minor);
  yy_destructor(66,&yymsp[-4].minor);
  yy_destructor(67,&yymsp[-2].minor);
}
// 3299 "parser.c"
        break;
      case 133:
// 1313 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_let_assignment("variable-append", yymsp[-1].minor.yy25, yymsp[-4].minor.yy0, NULL, NULL, yymsp[0].minor.yy25, status->scanner_state);
  yy_destructor(66,&yymsp[-3].minor);
  yy_destructor(67,&yymsp[-2].minor);
}
// 3308 "parser.c"
        break;
      case 134:
// 1318 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_let_assignment("array-index", yymsp[-1].minor.yy25, yymsp[-5].minor.yy0, NULL, yymsp[-3].minor.yy25, yymsp[0].minor.yy25, status->scanner_state);
  yy_destructor(66,&yymsp[-4].minor);
  yy_destructor(67,&yymsp[-2].minor);
}
// 3317 "parser.c"
        break;
      case 135:
// 1323 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_let_assignment("incr", NULL, yymsp[-1].minor.yy0, NULL, NULL, NULL, status->scanner_state);
  yy_destructor(68,&yymsp[0].minor);
}
// 3325 "parser.c"
        break;
      case 136:
// 1328 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_let_assignment("decr", NULL, yymsp[-1].minor.yy0, NULL, NULL, NULL, status->scanner_state);
  yy_destructor(69,&yymsp[0].minor);
}
// 3333 "parser.c"
        break;
      case 138:
// 1336 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_echo_statement(yymsp[-1].minor.yy25, status->scanner_state);
  yy_destructor(70,&yymsp[-2].minor);
  yy_destructor(27,&yymsp[0].minor);
}
// 3342 "parser.c"
        break;
      case 141:
// 1348 "parser.lemon"
{
	yygotominor.yy25 = yymsp[0].minor.yy25;;
}
// 3349 "parser.c"
        break;
      case 142:
// 1353 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_mcall_statement(yymsp[-1].minor.yy25, status->scanner_state);
  yy_destructor(27,&yymsp[0].minor);
}
// 3357 "parser.c"
        break;
      case 143:
// 1358 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_return_statement(yymsp[-1].minor.yy25, status->scanner_state);
  yy_destructor(71,&yymsp[-2].minor);
  yy_destructor(27,&yymsp[0].minor);
}
// 3366 "parser.c"
        break;
      case 144:
// 1363 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_unset_statement(
		xx_ret_literal(XX_T_IDENTIFIER, yymsp[-3].minor.yy0, status->scanner_state),
		xx_ret_literal(XX_T_IDENTIFIER, yymsp[-1].minor.yy0, status->scanner_state),
		status->scanner_state
	);
  yy_destructor(72,&yymsp[-4].minor);
  yy_destructor(65,&yymsp[-2].minor);
  yy_destructor(27,&yymsp[0].minor);
}
// 3380 "parser.c"
        break;
      case 145:
// 1372 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_unset_statement(
		xx_ret_literal(XX_T_IDENTIFIER, yymsp[-4].minor.yy0, status->scanner_state),
		xx_ret_literal(XX_T_IDENTIFIER, yymsp[-2].minor.yy0, status->scanner_state),
		status->scanner_state
	);
  yy_destructor(72,&yymsp[-5].minor);
  yy_destructor(66,&yymsp[-3].minor);
  yy_destructor(67,&yymsp[-1].minor);
  yy_destructor(27,&yymsp[0].minor);
}
// 3395 "parser.c"
        break;
      case 146:
// 1381 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_unset_statement(
		xx_ret_literal(XX_T_IDENTIFIER, yymsp[-4].minor.yy0, status->scanner_state),
		xx_ret_literal(XX_T_INTEGER, yymsp[-2].minor.yy0, status->scanner_state),
		status->scanner_state
	);
  yy_destructor(72,&yymsp[-5].minor);
  yy_destructor(66,&yymsp[-3].minor);
  yy_destructor(67,&yymsp[-1].minor);
  yy_destructor(27,&yymsp[0].minor);
}
// 3410 "parser.c"
        break;
      case 147:
// 1390 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_unset_statement(
		xx_ret_literal(XX_T_IDENTIFIER, yymsp[-4].minor.yy0, status->scanner_state),
		xx_ret_literal(XX_T_STRING, yymsp[-2].minor.yy0, status->scanner_state),
		status->scanner_state
	);
  yy_destructor(72,&yymsp[-5].minor);
  yy_destructor(66,&yymsp[-3].minor);
  yy_destructor(67,&yymsp[-1].minor);
  yy_destructor(27,&yymsp[0].minor);
}
// 3425 "parser.c"
        break;
      case 148:
// 1399 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_throw_exception(yymsp[-1].minor.yy0, NULL, status->scanner_state);
  yy_destructor(75,&yymsp[-2].minor);
  yy_destructor(27,&yymsp[0].minor);
}
// 3434 "parser.c"
        break;
      case 149:
// 1404 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_throw_exception(yymsp[-4].minor.yy0, yymsp[-2].minor.yy25, status->scanner_state);
  yy_destructor(75,&yymsp[-6].minor);
  yy_destructor(23,&yymsp[-5].minor);
  yy_destructor(37,&yymsp[-3].minor);
  yy_destructor(38,&yymsp[-1].minor);
  yy_destructor(27,&yymsp[0].minor);
}
// 3446 "parser.c"
        break;
      case 150:
// 1408 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_declare_statement(XX_T_TYPE_INTEGER, yymsp[-1].minor.yy25, status->scanner_state);
  yy_destructor(40,&yymsp[-2].minor);
  yy_destructor(27,&yymsp[0].minor);
}
// 3455 "parser.c"
        break;
      case 151:
// 1412 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_declare_statement(XX_T_TYPE_DOUBLE, yymsp[-1].minor.yy25, status->scanner_state);
  yy_destructor(41,&yymsp[-2].minor);
  yy_destructor(27,&yymsp[0].minor);
}
// 3464 "parser.c"
        break;
      case 152:
// 1416 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_declare_statement(XX_T_TYPE_STRING, yymsp[-1].minor.yy25, status->scanner_state);
  yy_destructor(43,&yymsp[-2].minor);
  yy_destructor(27,&yymsp[0].minor);
}
// 3473 "parser.c"
        break;
      case 153:
// 1420 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_declare_statement(XX_T_TYPE_BOOL, yymsp[-1].minor.yy25, status->scanner_state);
  yy_destructor(42,&yymsp[-2].minor);
  yy_destructor(27,&yymsp[0].minor);
}
// 3482 "parser.c"
        break;
      case 154:
// 1424 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_declare_statement(XX_T_TYPE_VAR, yymsp[-1].minor.yy25, status->scanner_state);
  yy_destructor(45,&yymsp[-2].minor);
  yy_destructor(27,&yymsp[0].minor);
}
// 3491 "parser.c"
        break;
      case 157:
// 1436 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_declare_variable(yymsp[0].minor.yy0, NULL, status->scanner_state);
}
// 3498 "parser.c"
        break;
      case 158:
// 1440 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_declare_variable(yymsp[-2].minor.yy0, yymsp[0].minor.yy25, status->scanner_state);
  yy_destructor(34,&yymsp[-1].minor);
}
// 3506 "parser.c"
        break;
      case 160:
// 1448 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_expr("not", yymsp[0].minor.yy25, NULL, NULL, status->scanner_state);
  yy_destructor(24,&yymsp[-1].minor);
}
// 3514 "parser.c"
        break;
      case 161:
// 1452 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_expr("isset", yymsp[0].minor.yy25, NULL, NULL, status->scanner_state);
  yy_destructor(19,&yymsp[-1].minor);
}
// 3522 "parser.c"
        break;
      case 162:
// 1456 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_expr("equals", yymsp[-2].minor.yy25, yymsp[0].minor.yy25, NULL, status->scanner_state);
  yy_destructor(8,&yymsp[-1].minor);
}
// 3530 "parser.c"
        break;
      case 163:
// 1460 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_expr("not-equals", yymsp[-2].minor.yy25, yymsp[0].minor.yy25, NULL, status->scanner_state);
  yy_destructor(15,&yymsp[-1].minor);
}
// 3538 "parser.c"
        break;
      case 164:
// 1464 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_expr("identical", yymsp[-2].minor.yy25, yymsp[0].minor.yy25, NULL, status->scanner_state);
  yy_destructor(9,&yymsp[-1].minor);
}
// 3546 "parser.c"
        break;
      case 165:
// 1468 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_expr("not-identical", yymsp[-2].minor.yy25, yymsp[0].minor.yy25, NULL, status->scanner_state);
  yy_destructor(14,&yymsp[-1].minor);
}
// 3554 "parser.c"
        break;
      case 166:
// 1472 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_expr("less", yymsp[-2].minor.yy25, yymsp[0].minor.yy25, NULL, status->scanner_state);
  yy_destructor(10,&yymsp[-1].minor);
}
// 3562 "parser.c"
        break;
      case 167:
// 1476 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_expr("greater", yymsp[-2].minor.yy25, yymsp[0].minor.yy25, NULL, status->scanner_state);
  yy_destructor(11,&yymsp[-1].minor);
}
// 3570 "parser.c"
        break;
      case 168:
// 1480 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_expr("less-equal", yymsp[-2].minor.yy25, yymsp[0].minor.yy25, NULL, status->scanner_state);
  yy_destructor(12,&yymsp[-1].minor);
}
// 3578 "parser.c"
        break;
      case 169:
// 1484 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_expr("greater-equal", yymsp[-2].minor.yy25, yymsp[0].minor.yy25, NULL, status->scanner_state);
  yy_destructor(13,&yymsp[-1].minor);
}
// 3586 "parser.c"
        break;
      case 170:
// 1488 "parser.lemon"
{
	yygotominor.yy25 = yymsp[-1].minor.yy25;
  yy_destructor(37,&yymsp[-2].minor);
  yy_destructor(38,&yymsp[0].minor);
}
// 3595 "parser.c"
        break;
      case 171:
// 1492 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_expr("property-access", xx_ret_literal(XX_T_IDENTIFIER, yymsp[-2].minor.yy0, status->scanner_state), xx_ret_literal(XX_T_IDENTIFIER, yymsp[0].minor.yy0, status->scanner_state), NULL, status->scanner_state);
  yy_destructor(65,&yymsp[-1].minor);
}
// 3603 "parser.c"
        break;
      case 172:
      case 218:
// 1496 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_expr("array-access", xx_ret_literal(XX_T_IDENTIFIER, yymsp[-3].minor.yy0, status->scanner_state), xx_ret_literal(XX_T_IDENTIFIER, yymsp[-1].minor.yy0, status->scanner_state), NULL, status->scanner_state);
  yy_destructor(66,&yymsp[-2].minor);
  yy_destructor(67,&yymsp[0].minor);
}
// 3613 "parser.c"
        break;
      case 173:
      case 219:
// 1501 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_expr("array-access", xx_ret_literal(XX_T_IDENTIFIER, yymsp[-3].minor.yy0, status->scanner_state), xx_ret_literal(XX_T_INTEGER, yymsp[-1].minor.yy0, status->scanner_state), NULL, status->scanner_state);
  yy_destructor(66,&yymsp[-2].minor);
  yy_destructor(67,&yymsp[0].minor);
}
// 3623 "parser.c"
        break;
      case 174:
      case 220:
// 1506 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_expr("array-access", xx_ret_literal(XX_T_IDENTIFIER, yymsp[-3].minor.yy0, status->scanner_state), xx_ret_literal(XX_T_STRING, yymsp[-1].minor.yy0, status->scanner_state), NULL, status->scanner_state);
  yy_destructor(66,&yymsp[-2].minor);
  yy_destructor(67,&yymsp[0].minor);
}
// 3633 "parser.c"
        break;
      case 175:
// 1511 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_expr("add", yymsp[-2].minor.yy25, yymsp[0].minor.yy25, NULL, status->scanner_state);
  yy_destructor(16,&yymsp[-1].minor);
}
// 3641 "parser.c"
        break;
      case 176:
// 1516 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_expr("sub", yymsp[-2].minor.yy25, yymsp[0].minor.yy25, NULL, status->scanner_state);
  yy_destructor(17,&yymsp[-1].minor);
}
// 3649 "parser.c"
        break;
      case 177:
// 1521 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_expr("concat", yymsp[-2].minor.yy25, yymsp[0].minor.yy25, NULL, status->scanner_state);
  yy_destructor(18,&yymsp[-1].minor);
}
// 3657 "parser.c"
        break;
      case 178:
// 1526 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_expr("and", yymsp[-2].minor.yy25, yymsp[0].minor.yy25, NULL, status->scanner_state);
  yy_destructor(6,&yymsp[-1].minor);
}
// 3665 "parser.c"
        break;
      case 179:
// 1531 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_expr("or", yymsp[-2].minor.yy25, yymsp[0].minor.yy25, NULL, status->scanner_state);
  yy_destructor(7,&yymsp[-1].minor);
}
// 3673 "parser.c"
        break;
      case 180:
// 1536 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_expr("instanceof", yymsp[0].minor.yy25, NULL, NULL, status->scanner_state);
  yy_destructor(22,&yymsp[-1].minor);
}
// 3681 "parser.c"
        break;
      case 181:
// 1541 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_expr("fetch", xx_ret_literal(XX_T_IDENTIFIER, yymsp[-2].minor.yy0, status->scanner_state), yymsp[0].minor.yy25, NULL, status->scanner_state);
  yy_destructor(20,&yymsp[-3].minor);
  yy_destructor(5,&yymsp[-1].minor);
}
// 3690 "parser.c"
        break;
      case 182:
// 1546 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_expr("typeof", yymsp[0].minor.yy25, NULL, NULL, status->scanner_state);
  yy_destructor(21,&yymsp[-1].minor);
}
// 3698 "parser.c"
        break;
      case 183:
      case 207:
      case 211:
// 1551 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_literal(XX_T_IDENTIFIER, yymsp[0].minor.yy0, status->scanner_state);
}
// 3707 "parser.c"
        break;
      case 184:
      case 209:
      case 212:
// 1556 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_literal(XX_T_INTEGER, yymsp[0].minor.yy0, status->scanner_state);
}
// 3716 "parser.c"
        break;
      case 185:
      case 208:
      case 213:
// 1561 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_literal(XX_T_STRING, yymsp[0].minor.yy0, status->scanner_state);
}
// 3725 "parser.c"
        break;
      case 186:
      case 214:
// 1566 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_literal(XX_T_DOUBLE, yymsp[0].minor.yy0, status->scanner_state);
}
// 3733 "parser.c"
        break;
      case 187:
      case 215:
// 1571 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_literal(XX_T_NULL, NULL, status->scanner_state);
  yy_destructor(77,&yymsp[0].minor);
}
// 3742 "parser.c"
        break;
      case 188:
      case 217:
// 1576 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_literal(XX_T_TRUE, NULL, status->scanner_state);
  yy_destructor(78,&yymsp[0].minor);
}
// 3751 "parser.c"
        break;
      case 189:
      case 216:
// 1581 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_literal(XX_T_FALSE, NULL, status->scanner_state);
  yy_destructor(79,&yymsp[0].minor);
}
// 3760 "parser.c"
        break;
      case 190:
// 1586 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_expr("empty-array", NULL, NULL, NULL, status->scanner_state);
  yy_destructor(66,&yymsp[-1].minor);
  yy_destructor(67,&yymsp[0].minor);
}
// 3769 "parser.c"
        break;
      case 191:
// 1591 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_expr("array", yymsp[-1].minor.yy25, NULL, NULL, status->scanner_state);
  yy_destructor(66,&yymsp[-2].minor);
  yy_destructor(67,&yymsp[0].minor);
}
// 3778 "parser.c"
        break;
      case 192:
// 1596 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_new_instance(yymsp[0].minor.yy0, NULL, status->scanner_state);
  yy_destructor(23,&yymsp[-1].minor);
}
// 3786 "parser.c"
        break;
      case 193:
// 1601 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_new_instance(yymsp[-2].minor.yy0, NULL, status->scanner_state);
  yy_destructor(23,&yymsp[-3].minor);
  yy_destructor(37,&yymsp[-1].minor);
  yy_destructor(38,&yymsp[0].minor);
}
// 3796 "parser.c"
        break;
      case 194:
// 1606 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_new_instance(yymsp[-3].minor.yy0, yymsp[-1].minor.yy25, status->scanner_state);
  yy_destructor(23,&yymsp[-4].minor);
  yy_destructor(37,&yymsp[-2].minor);
  yy_destructor(38,&yymsp[0].minor);
}
// 3806 "parser.c"
        break;
      case 195:
// 1611 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_fcall(yymsp[-3].minor.yy0, yymsp[-1].minor.yy25, status->scanner_state);
  yy_destructor(37,&yymsp[-2].minor);
  yy_destructor(38,&yymsp[0].minor);
}
// 3815 "parser.c"
        break;
      case 196:
// 1616 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_fcall(yymsp[-2].minor.yy0, NULL, status->scanner_state);
  yy_destructor(37,&yymsp[-1].minor);
  yy_destructor(38,&yymsp[0].minor);
}
// 3824 "parser.c"
        break;
      case 198:
// 1626 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_mcall(yymsp[-5].minor.yy0, yymsp[-3].minor.yy0, yymsp[-1].minor.yy25, status->scanner_state);
  yy_destructor(65,&yymsp[-4].minor);
  yy_destructor(37,&yymsp[-2].minor);
  yy_destructor(38,&yymsp[0].minor);
}
// 3834 "parser.c"
        break;
      case 199:
// 1631 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_mcall(yymsp[-4].minor.yy0, yymsp[-2].minor.yy0, NULL, status->scanner_state);
  yy_destructor(65,&yymsp[-3].minor);
  yy_destructor(37,&yymsp[-1].minor);
  yy_destructor(38,&yymsp[0].minor);
}
// 3844 "parser.c"
        break;
      case 205:
// 1655 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_array_item(yymsp[-2].minor.yy25, yymsp[0].minor.yy25, status->scanner_state);
  yy_destructor(52,&yymsp[-1].minor);
}
// 3852 "parser.c"
        break;
      case 206:
// 1659 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_array_item(NULL, yymsp[0].minor.yy25, status->scanner_state);
}
// 3859 "parser.c"
        break;
      case 222:
// 1726 "parser.lemon"
{
	yygotominor.yy25 = xx_ret_comment(yymsp[0].minor.yy0, status->scanner_state);
}
// 3866 "parser.c"
        break;
  };
  yygoto = yyRuleInfo[yyruleno].lhs;
  yysize = yyRuleInfo[yyruleno].nrhs;
  yypParser->yyidx -= yysize;
  yyact = yy_find_reduce_action(yypParser,yygoto);
  if( yyact < YYNSTATE ){
    yy_shift(yypParser,yyact,yygoto,&yygotominor);
  }else if( yyact == YYNSTATE + YYNRULE + 1 ){
    yy_accept(yypParser);
  }
}

/*
** The following code executes when the parse fails
*/
static void yy_parse_failed(
  yyParser *yypParser           /* The parser */
){
  xx_ARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
  xx_ARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  int yymajor,                   /* The major type of the error token */
  YYMINORTYPE yyminor            /* The minor type of the error token */
){
  xx_ARG_FETCH;
#define TOKEN (yyminor.yy0)
// 724 "parser.lemon"


	//fprintf(stderr, "error!\n");

	json_object *syntax_error = json_object_new_object();

	json_object_object_add(syntax_error, "type", json_object_new_string("error"));

	/*if (status->scanner_state->start_length) {
		fprintf(stderr, "Syntax error, %s", status->scanner_state->start);
	} else {
		fprintf(stderr, "EOF");
	}*/

	//status->syntax_error_len = 48 + Z_STRLEN_P(status->scanner_state->active_file);
	//status->syntax_error = emalloc(sizeof(char) * status->syntax_error_len);

	if (status->scanner_state->start_length) {
		json_object_object_add(syntax_error, "message", json_object_new_string("Syntax error"));
	} else {
		json_object_object_add(syntax_error, "message", json_object_new_string("Unexpected EOF"));
	}

	json_object_object_add(syntax_error, "file", json_object_new_string(status->scanner_state->active_file));
	json_object_object_add(syntax_error, "line", json_object_new_int(status->scanner_state->active_line));
	json_object_object_add(syntax_error, "char", json_object_new_int(status->scanner_state->active_char));

	status->status = XX_PARSING_FAILED;

	status->ret = syntax_error;

	//status->scanner_state->active_file

// 3942 "parser.c"
  xx_ARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
  yyParser *yypParser           /* The parser */
){
  xx_ARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sAccept!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
  xx_ARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "xx_Alloc" which describes the current state of the parser.
** The second argument is the major token number.  The third is
** the minor token.  The fourth optional argument is whatever the
** user wants (and specified in the grammar) and is available for
** use by the action routines.
**
** Inputs:
** <ul>
** <li> A pointer to the parser (an opaque structure.)
** <li> The major token number.
** <li> The minor token number.
** <li> An option argument of a grammar-specified type.
** </ul>
**
** Outputs:
** None.
*/
void xx_(
  void *yyp,                   /* The parser */
  int yymajor,                 /* The major token code number */
  xx_TOKENTYPE yyminor       /* The value for the token */
  xx_ARG_PDECL               /* Optional %extra_argument parameter */
){
  YYMINORTYPE yyminorunion;
  int yyact;            /* The parser action. */
  int yyendofinput;     /* True if we are at the end of input */
  int yyerrorhit = 0;   /* True if yymajor has invoked an error */
  yyParser *yypParser;  /* The parser */

  /* (re)initialize the parser, if necessary */
  yypParser = (yyParser*)yyp;
  if( yypParser->yyidx<0 ){
    if( yymajor==0 ) return;
    yypParser->yyidx = 0;
    yypParser->yyerrcnt = -1;
    yypParser->yystack[0].stateno = 0;
    yypParser->yystack[0].major = 0;
  }
  yyminorunion.yy0 = yyminor;
  yyendofinput = (yymajor==0);
  xx_ARG_STORE;

#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sInput %s\n",yyTracePrompt,yyTokenName[yymajor]);
  }
#endif

  do{
    yyact = yy_find_shift_action(yypParser,yymajor);
    if( yyact<YYNSTATE ){
      yy_shift(yypParser,yyact,yymajor,&yyminorunion);
      yypParser->yyerrcnt--;
      if( yyendofinput && yypParser->yyidx>=0 ){
        yymajor = 0;
      }else{
        yymajor = YYNOCODE;
      }
    }else if( yyact < YYNSTATE + YYNRULE ){
      yy_reduce(yypParser,yyact-YYNSTATE);
    }else if( yyact == YY_ERROR_ACTION ){
      int yymx;
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE,"%sSyntax Error!\n",yyTracePrompt);
      }
#endif
#ifdef YYERRORSYMBOL
      /* A syntax error has occurred.
      ** The response to an error depends upon whether or not the
      ** grammar defines an error token "ERROR".  
      **
      ** This is what we do if the grammar does define ERROR:
      **
      **  * Call the %syntax_error function.
      **
      **  * Begin popping the stack until we enter a state where
      **    it is legal to shift the error symbol, then shift
      **    the error symbol.
      **
      **  * Set the error count to three.
      **
      **  * Begin accepting and shifting new tokens.  No new error
      **    processing will occur until three tokens have been
      **    shifted successfully.
      **
      */
      if( yypParser->yyerrcnt<0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yymx = yypParser->yystack[yypParser->yyidx].major;
      if( yymx==YYERRORSYMBOL || yyerrorhit ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE,"%sDiscard input token %s\n",
             yyTracePrompt,yyTokenName[yymajor]);
        }
#endif
        yy_destructor(yymajor,&yyminorunion);
        yymajor = YYNOCODE;
      }else{
         while(
          yypParser->yyidx >= 0 &&
          yymx != YYERRORSYMBOL &&
          (yyact = yy_find_shift_action(yypParser,YYERRORSYMBOL)) >= YYNSTATE
        ){
          yy_pop_parser_stack(yypParser);
        }
        if( yypParser->yyidx < 0 || yymajor==0 ){
          yy_destructor(yymajor,&yyminorunion);
          yy_parse_failed(yypParser);
          yymajor = YYNOCODE;
        }else if( yymx!=YYERRORSYMBOL ){
          YYMINORTYPE u2;
          u2.YYERRSYMDT = 0;
          yy_shift(yypParser,yyact,YYERRORSYMBOL,&u2);
        }
      }
      yypParser->yyerrcnt = 3;
      yyerrorhit = 1;
#else  /* YYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if( yypParser->yyerrcnt<=0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yypParser->yyerrcnt = 3;
      yy_destructor(yymajor,&yyminorunion);
      if( yyendofinput ){
        yy_parse_failed(yypParser);
      }
      yymajor = YYNOCODE;
#endif
    }else{
      yy_accept(yypParser);
      yymajor = YYNOCODE;
    }
  }while( yymajor!=YYNOCODE && yypParser->yyidx>=0 );
  return;
}

/*
 +----------------------------------------------------------------------+
 | Zephir Language                                                      |
 +----------------------------------------------------------------------+
 | Copyright (c) 2013 Zephir Team                                       |
 +----------------------------------------------------------------------+
 | This source file is subject to version 1.0 of the Zephir license,    |
 | that is bundled with this package in the file LICENSE, and is        |
 | available through the world-wide-web at the following url:           |
 | http://www.zephir-lang.com/license                                   |
 |                                                                      |
 | If you did not receive a copy of the Zephir license and are unable   |
 | to obtain it through the world-wide-web, please send a note to       |
 | license@zephir-lang.com so we can mail you a copy immediately.       |
 +----------------------------------------------------------------------+
*/

#define SUCCESS 1
#define FAILURE 0

const xx_token_names xx_tokens[] =
{
	{ XX_T_INTEGER,       		"INTEGER" },
	{ XX_T_DOUBLE,        		"DOUBLE" },
	{ XX_T_STRING,        		"STRING" },
	{ XX_T_IDENTIFIER,    		"IDENTIFIER" },
	{ XX_T_AT,	   	    		"@" },
	{ XX_T_COMMA,	   	    	"," },
	{ XX_T_ASSIGN,       		"=" },
	{ XX_T_COLON,            	":" },
	{ XX_T_PARENTHESES_OPEN,  	"(" },
	{ XX_T_PARENTHESES_CLOSE, 	")" },
	{ XX_T_BRACKET_OPEN,     	"{" },
	{ XX_T_BRACKET_CLOSE,    	"}" },
 	{ XX_T_SBRACKET_OPEN,    	"[" },
	{ XX_T_SBRACKET_CLOSE,   	"]" },
	{  0, NULL }
};

/**
 * Wrapper to alloc memory within the parser
 */
static void *xx_wrapper_alloc(size_t bytes){
	return malloc(bytes);
}

/**
 * Wrapper to free memory within the parser
 */
static void xx_wrapper_free(void *pointer){
	//free(pointer);
}

/**
 * Creates a parser_token to be passed to the parser
 */
static void xx_parse_with_token(void* xx_parser, int opcode, int parsercode, xx_scanner_token *token, xx_parser_status *parser_status){

	xx_parser_token *pToken;

	pToken = malloc(sizeof(xx_parser_token));
	pToken->opcode = opcode;
	pToken->token = token->value;
	pToken->token_len = token->len;
	pToken->free_flag = 1;

	xx_(xx_parser, parsercode, pToken, parser_status);

	token->value = NULL;
	token->len = 0;
}

/**
 * Creates an error message when it's triggered by the scanner
 */
static void xx_scanner_error_msg(xx_parser_status *parser_status){

	/*char *error, *error_part;
	XX_scanner_state *state = parser_status->scanner_state;

	//PHALCON_INIT_VAR(*error_msg);
	ALLOC_INIT_ZVAL(*error_msg);
	if (state->start) {
		error = emalloc(sizeof(char) * (128 + state->start_length +  Z_STRLEN_P(state->active_file)));
		if (state->start_length > 16) {
			error_part = estrndup(state->start, 16);
			sprintf(error, "Parsing error before '%s...' in %s on line %d", error_part, Z_STRVAL_P(state->active_file), state->active_line);
			efree(error_part);
		} else {
			sprintf(error, "Parsing error before '%s' in %s on line %d", state->start, Z_STRVAL_P(state->active_file), state->active_line);
		}
		ZVAL_STRING(*error_msg, error, 1);
	} else {
		error = emalloc(sizeof(char) * (64 + Z_STRLEN_P(state->active_file)));
		sprintf(error, "Parsing error near to EOF in %s", Z_STRVAL_P(state->active_file));
		ZVAL_STRING(*error_msg, error, 1);
	}
	efree(error);*/
}

/**
 * Parses a comment returning an intermediate array representation
 */
int xx_parse_program(char *program, unsigned int program_length, char *file_path) {

	char *error;
	xx_scanner_state *state;
	xx_scanner_token token;
	int scanner_status, status = SUCCESS, start_lines;
	xx_parser_status *parser_status = NULL;
	void* xx_parser;

	/**
	 * Check if the program has any length
	 */
	if (!program_length) {
		return FAILURE;
	}

	if (program_length < 2) {
		return SUCCESS;
	}

	/**
	 * Start the reentrant parser
	 */
	xx_parser = xx_Alloc(xx_wrapper_alloc);

	parser_status = malloc(sizeof(xx_parser_status));
	state = malloc(sizeof(xx_scanner_state));

	parser_status->status = XX_PARSING_OK;
	parser_status->scanner_state = state;
	parser_status->ret = NULL;
	parser_status->token = &token;
	parser_status->syntax_error = NULL;

	/**
	 * Initialize the scanner state
	 */
	state->active_token = 0;
	state->start = program;
	state->start_length = 0;
	state->active_file = file_path;
	state->active_line = 1;
	state->active_char = 1;

	state->end = state->start;

	while (0 <= (scanner_status = xx_get_token(state, &token))) {

		state->active_token = token.opcode;

		state->start_length = (program + program_length - state->start);

		switch (token.opcode) {

			case XX_T_IGNORE:
				break;

			case XX_T_NAMESPACE:
				xx_(xx_parser, XX_NAMESPACE, NULL, parser_status);
				break;
			case XX_T_ABSTRACT:
				xx_(xx_parser, XX_ABSTRACT, NULL, parser_status);
				break;
			case XX_T_CLASS:
				xx_(xx_parser, XX_CLASS, NULL, parser_status);
				break;
			case XX_T_EXTENDS:
				xx_(xx_parser, XX_EXTENDS, NULL, parser_status);
				break;
			case XX_T_PUBLIC:
				xx_(xx_parser, XX_PUBLIC, NULL, parser_status);
				break;
			case XX_T_PROTECTED:
				xx_(xx_parser, XX_PROTECTED, NULL, parser_status);
				break;
			case XX_T_PRIVATE:
				xx_(xx_parser, XX_PRIVATE, NULL, parser_status);
				break;
			case XX_T_STATIC:
				xx_(xx_parser, XX_STATIC, NULL, parser_status);
				break;
			case XX_T_INLINE:
				xx_(xx_parser, XX_INLINE, NULL, parser_status);
				break;
			case XX_T_FUNCTION:
				xx_(xx_parser, XX_FUNCTION, NULL, parser_status);
				break;
			case XX_T_LET:
				xx_(xx_parser, XX_LET, NULL, parser_status);
				break;
			case XX_T_ECHO:
				xx_(xx_parser, XX_ECHO, NULL, parser_status);
				break;
			case XX_T_RETURN:
				xx_(xx_parser, XX_RETURN, NULL, parser_status);
				break;
			case XX_T_IF:
				xx_(xx_parser, XX_IF, NULL, parser_status);
				break;
			case XX_T_ELSE:
				xx_(xx_parser, XX_ELSE, NULL, parser_status);
				break;
			case XX_T_LOOP:
				xx_(xx_parser, XX_LOOP, NULL, parser_status);
				break;
			case XX_T_CONTINUE:
				xx_(xx_parser, XX_CONTINUE, NULL, parser_status);
				break;
			case XX_T_BREAK:
				xx_(xx_parser, XX_BREAK, NULL, parser_status);
				break;
			case XX_T_WHILE:
				xx_(xx_parser, XX_WHILE, NULL, parser_status);
				break;
			case XX_T_NEW:
				xx_(xx_parser, XX_NEW, NULL, parser_status);
				break;
			case XX_T_CONST:
				xx_(xx_parser, XX_CONST, NULL, parser_status);
				break;
			case XX_T_TYPEOF:
				xx_(xx_parser, XX_TYPEOF, NULL, parser_status);
				break;
			case XX_T_INSTANCEOF:
				xx_(xx_parser, XX_INSTANCEOF, NULL, parser_status);
				break;
			case XX_T_ISSET:
				xx_(xx_parser, XX_ISSET, NULL, parser_status);
				break;
			case XX_T_UNSET:
				xx_(xx_parser, XX_UNSET, NULL, parser_status);
				break;
			case XX_T_THROW:
				xx_(xx_parser, XX_THROW, NULL, parser_status);
				break;
			case XX_T_FOR:
				xx_(xx_parser, XX_FOR, NULL, parser_status);
				break;
			case XX_T_IN:
				xx_(xx_parser, XX_IN, NULL, parser_status);
				break;
			case XX_T_REVERSE:
				xx_(xx_parser, XX_REVERSE, NULL, parser_status);
				break;

			case XX_T_DOTCOMMA:
				xx_(xx_parser, XX_DOTCOMMA, NULL, parser_status);
				break;
			case XX_T_COMMA:
				xx_(xx_parser, XX_COMMA, NULL, parser_status);
				break;
			case XX_T_ASSIGN:
				xx_(xx_parser, XX_ASSIGN, NULL, parser_status);
				break;
			case XX_T_ADDASSIGN:
				xx_(xx_parser, XX_ADDASSIGN, NULL, parser_status);
				break;
			case XX_T_SUBASSIGN:
				xx_(xx_parser, XX_SUBASSIGN, NULL, parser_status);
				break;
			case XX_T_MULASSIGN:
				xx_(xx_parser, XX_MULASSIGN, NULL, parser_status);
				break;
			case XX_T_CONCATASSIGN:
				xx_(xx_parser, XX_CONCATASSIGN, NULL, parser_status);
				break;
			case XX_T_EQUALS:
				xx_(xx_parser, XX_EQUALS, NULL, parser_status);
				break;
			case XX_T_NOTEQUALS:
				xx_(xx_parser, XX_NOTEQUALS, NULL, parser_status);
				break;
			case XX_T_IDENTICAL:
				xx_(xx_parser, XX_IDENTICAL, NULL, parser_status);
				break;
			case XX_T_NOTIDENTICAL:
				xx_(xx_parser, XX_NOTIDENTICAL, NULL, parser_status);
				break;
			case XX_T_LESS:
				xx_(xx_parser, XX_LESS, NULL, parser_status);
				break;
			case XX_T_GREATER:
				xx_(xx_parser, XX_GREATER, NULL, parser_status);
				break;
			case XX_T_LESSEQUAL:
				xx_(xx_parser, XX_LESSEQUAL, NULL, parser_status);
				break;
			case XX_T_GREATEREQUAL:
				xx_(xx_parser, XX_GREATEREQUAL, NULL, parser_status);
				break;
			case XX_T_COLON:
				xx_(xx_parser, XX_COLON, NULL, parser_status);
				break;
			case XX_T_ARROW:
				xx_(xx_parser, XX_ARROW, NULL, parser_status);
				break;
			case XX_T_NOT:
				xx_(xx_parser, XX_NOT, NULL, parser_status);
				break;
			case XX_T_FETCH:
				xx_(xx_parser, XX_FETCH, NULL, parser_status);
				break;
			case XX_T_SWITCH:
				xx_(xx_parser, XX_SWITCH, NULL, parser_status);
				break;
			case XX_T_CASE:
				xx_(xx_parser, XX_CASE, NULL, parser_status);
				break;
			case XX_T_DEFAULT:
				xx_(xx_parser, XX_DEFAULT, NULL, parser_status);
				break;

			case XX_T_PARENTHESES_OPEN:
				xx_(xx_parser, XX_PARENTHESES_OPEN, NULL, parser_status);
				break;
			case XX_T_PARENTHESES_CLOSE:
				xx_(xx_parser, XX_PARENTHESES_CLOSE, NULL, parser_status);
				break;

			case XX_T_BRACKET_OPEN:
				xx_(xx_parser, XX_BRACKET_OPEN, NULL, parser_status);
				break;
			case XX_T_BRACKET_CLOSE:
				xx_(xx_parser, XX_BRACKET_CLOSE, NULL, parser_status);
				break;

			case XX_T_SBRACKET_OPEN:
				xx_(xx_parser, XX_SBRACKET_OPEN, NULL, parser_status);
				break;
			case XX_T_SBRACKET_CLOSE:
				xx_(xx_parser, XX_SBRACKET_CLOSE, NULL, parser_status);
				break;

			case XX_T_NULL:
				xx_(xx_parser, XX_NULL, NULL, parser_status);
				break;
			case XX_T_TRUE:
				xx_(xx_parser, XX_TRUE, NULL, parser_status);
				break;
			case XX_T_FALSE:
				xx_(xx_parser, XX_FALSE, NULL, parser_status);
				break;
			case XX_T_COMMENT:
				xx_parse_with_token(xx_parser, XX_T_COMMENT, XX_COMMENT, &token, parser_status);
				break;

			case XX_T_TYPE_INTEGER:
				xx_(xx_parser, XX_TYPE_INTEGER, NULL, parser_status);
				break;
			case XX_T_TYPE_DOUBLE:
				xx_(xx_parser, XX_TYPE_DOUBLE, NULL, parser_status);
				break;
			case XX_T_TYPE_STRING:
				xx_(xx_parser, XX_TYPE_STRING, NULL, parser_status);
				break;
			case XX_T_TYPE_BOOL:
				xx_(xx_parser, XX_TYPE_BOOL, NULL, parser_status);
				break;
			case XX_T_TYPE_VAR:
				xx_(xx_parser, XX_TYPE_VAR, NULL, parser_status);
				break;

			case XX_T_ADD:
				xx_(xx_parser, XX_ADD, NULL, parser_status);
				break;
			case XX_T_SUB:
				xx_(xx_parser, XX_SUB, NULL, parser_status);
				break;
			case XX_T_DOT:
				xx_(xx_parser, XX_CONCAT, NULL, parser_status);
				break;
			case XX_T_INCR:
				xx_(xx_parser, XX_INCR, NULL, parser_status);
				break;
			case XX_T_DECR:
				xx_(xx_parser, XX_DECR, NULL, parser_status);
				break;
			case XX_T_AND:
				xx_(xx_parser, XX_AND, NULL, parser_status);
				break;
			case XX_T_OR:
				xx_(xx_parser, XX_OR, NULL, parser_status);
				break;

			case XX_T_INTEGER:
				xx_parse_with_token(xx_parser, XX_T_INTEGER, XX_INTEGER, &token, parser_status);
				break;
			case XX_T_DOUBLE:
				xx_parse_with_token(xx_parser, XX_T_DOUBLE, XX_DOUBLE, &token, parser_status);
				break;
			case XX_T_STRING:
				xx_parse_with_token(xx_parser, XX_T_STRING, XX_STRING, &token, parser_status);
				break;
			case XX_T_IDENTIFIER:
				xx_parse_with_token(xx_parser, XX_T_IDENTIFIER, XX_IDENTIFIER, &token, parser_status);
				break;

			default:
				parser_status->status = XX_PARSING_FAILED;
				fprintf(stderr, "Scanner: unknown opcode %d\n", token.opcode);
				/*if (!*error_msg) {
					error = emalloc(sizeof(char) * (48 + Z_STRLEN_P(state->active_file)));
					sprintf(error, "Scanner: unknown opcode %d on in %s line %d", token.opcode, Z_STRVAL_P(state->active_file), state->active_line);
					//PHALCON_INIT_VAR(*error_msg);
					ALLOC_INIT_ZVAL(*error_msg);
					ZVAL_STRING(*error_msg, error, 1);
					efree(error);
				}*/
				break;
		}

		if (parser_status->status != XX_PARSING_OK) {
			status = FAILURE;
			break;
		}

		state->end = state->start;
	}

	if (status != FAILURE) {
		switch (scanner_status) {
			case XX_SCANNER_RETCODE_ERR:
			case XX_SCANNER_RETCODE_IMPOSSIBLE:
				{
					char *x = malloc(sizeof(char) * 10000);
					if (state->start) {
						sprintf(x, "Scanner error: %d %s", scanner_status, state->start);
					} else {
						sprintf(x, "Scanner error: %d", scanner_status);
					}
					json_object *syntax_error = json_object_new_object();
					json_object_object_add(syntax_error, "type", json_object_new_string("error"));
					json_object_object_add(syntax_error, "message", json_object_new_string(x));
					//free(x);
					parser_status->ret = syntax_error;
					status = FAILURE;
				}
				break;
			default:
				xx_(xx_parser, 0, NULL, parser_status);
		}
	}

	state->active_token = 0;
	state->start = NULL;

	if (parser_status->status != XX_PARSING_OK) {
		status = FAILURE;
		/*if (parser_status->syntax_error) {
			if (!*error_msg) {
				//PHALCON_INIT_VAR(*error_msg);
				ALLOC_INIT_ZVAL(*error_msg);
				ZVAL_STRING(*error_msg, parser_status->syntax_error, 1);
			}
			efree(parser_status->syntax_error);
		}*/
		//fprintf(stderr, "error!\n");
	}

	xx_Free(xx_parser, xx_wrapper_free);

	if (status != FAILURE) {
		if (parser_status->status == XX_PARSING_OK) {
			//printf("%s\n", json_object_to_json_string(parser_status->ret));
			/*if (parser_status->ret) {
				ZVAL_ZVAL(*result, parser_status->ret, 0, 0);
				ZVAL_NULL(parser_status->ret);
				zval_ptr_dtor(&parser_status->ret);
			} else {
				array_init(*result);
			}*/
		}
	}

	if (parser_status->ret) {
		printf("%s\n", json_object_to_json_string(parser_status->ret));
	}

	//efree(Z_STRVAL(processed_comment));*/

	free(parser_status);
	free(state);

	return status;
}

int main(int argc, char **argv) {

	FILE *fp;
	char ch;
	char *program;
	int i, length;

	if (argc > 0) {

		fp = fopen(argv[1], "r");
		if (!fp) {
			fprintf(stderr, "Cant open file\n");
			exit(1);
		}

		length = 1024;
		program = malloc(sizeof(char) * length);

		i = 0;
		while (!feof(fp)) {
			ch = fgetc(fp);
			if (i == length) {
				length += 1024;
				program = realloc(program, sizeof(char) * length);
			}
			program[i++] = ch;
		}
		program[i - 1] = '\0';
		fclose(fp);

		xx_parse_program(program, i - 1, argv[1]);

		free(program);
	}

}
