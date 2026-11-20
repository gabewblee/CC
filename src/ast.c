#include <string.h>

#include "ast.h"

static const char *copy(Arena *arena, const char *name, size_t length) {
    if (!name) return NULL;
    return arena_strndup(arena, name, length);
}

static Type *ast_build_type(Arena *arena, TypeKind kind, bool constant) {
    Type *type = arena_alloc(arena, sizeof *type);
    memset(type, 0, sizeof *type);
    type->kind     = kind;
    type->constant = constant;
    return type;
}

static Node *ast_build_node(Arena *arena, NodeKind kind, Location location) {
    Node *node = arena_alloc(arena, sizeof *node);
    memset(node, 0, sizeof *node);
    node->kind     = kind;
    node->location = location;
    return node;
}

Type *ast_build_void_type(Arena *arena, bool constant) {
    return ast_build_type(arena, TypeVoid, constant);
}

Type *ast_build_bool_type(Arena *arena, bool constant) {
    return ast_build_type(arena, TypeBool, constant);
}

Type *ast_build_char_type(Arena *arena, bool constant) {
    return ast_build_type(arena, TypeChar, constant);
}

Type *ast_build_int_type(Arena *arena, bool constant) {
    return ast_build_type(arena, TypeInt, constant);
}

Type *ast_build_pointer_type(Arena *arena, Type *base, bool constant) {
    Type *type         = ast_build_type(arena, TypePointer, constant);
    type->pointer.base = base;
    return type;
}

Type *ast_build_array_type(Arena *arena, Type *element, Node *length, bool constant) {
    Type *type          = ast_build_type(arena, TypeArray, constant);
    type->array.element = element;
    type->array.length  = length;
    return type;
}

Type *ast_build_function_type(Arena *arena, Type *result, Node *parameters) {
    Type *type                = ast_build_type(arena, TypeFunction, false);
    type->function.result     = result;
    type->function.parameters = parameters;
    return type;
}

Type *ast_build_struct_type(Arena *arena, const char *name, size_t length, Node *fields, bool constant) {
    Type *type          = ast_build_type(arena, TypeStruct, constant);
    type->record.name   = copy(arena, name, length);
    type->record.fields = fields;
    return type;
}

Type *ast_build_union_type(Arena *arena, const char *name, size_t length, Node *fields, bool constant) {
    Type *type          = ast_build_type(arena, TypeUnion, constant);
    type->record.name   = copy(arena, name, length);
    type->record.fields = fields;
    return type;
}

Type *ast_build_enum_type(Arena *arena, const char *name, size_t length, Node *enumerators, bool constant) {
    Type *type                    = ast_build_type(arena, TypeEnum, constant);
    type->enumeration.name        = copy(arena, name, length);
    type->enumeration.enumerators = enumerators;
    return type;
}

Node *ast_build_integer_node(Arena *arena, Location location, unsigned long long value) {
    Node *node          = ast_build_node(arena, NodeInteger, location);
    node->integer.value = value;
    return node;
}

Node *ast_build_character_node(Arena *arena, Location location, int value) {
    Node *node            = ast_build_node(arena, NodeCharacter, location);
    node->character.value = value;
    return node;
}

Node *ast_build_identifier_node(Arena *arena, Location location, const char *name, size_t length) {
    Node *node            = ast_build_node(arena, NodeIdentifier, location);
    node->identifier.name = copy(arena, name, length);
    return node;
}

Node *ast_build_unary_node(Arena *arena, Location location, TokenKind op, Node *operand) {
    Node *node          = ast_build_node(arena, NodeUnary, location);
    node->unary.op      = op;
    node->unary.operand = operand;
    return node;
}

Node *ast_build_binary_node(Arena *arena, Location location, TokenKind op, Node *left, Node *right) {
    Node *node         = ast_build_node(arena, NodeBinary, location);
    node->binary.op    = op;
    node->binary.left  = left;
    node->binary.right = right;
    return node;
}

Node *ast_build_ternary_node(Arena *arena, Location location, Node *condition, Node *then, Node *otherwise) {
    Node *node              = ast_build_node(arena, NodeTernary, location);
    node->ternary.condition = condition;
    node->ternary.then      = then;
    node->ternary.otherwise = otherwise;
    return node;
}

Node *ast_build_call_node(Arena *arena, Location location, Node *callee, Node *arguments) {
    Node *node           = ast_build_node(arena, NodeCall, location);
    node->call.callee    = callee;
    node->call.arguments = arguments;
    return node;
}

Node *ast_build_index_node(Arena *arena, Location location, Node *object, Node *index) {
    Node *node             = ast_build_node(arena, NodeIndex, location);
    node->subscript.object = object;
    node->subscript.index  = index;
    return node;
}

Node *ast_build_member_node(Arena *arena, Location location, Node *object, const char *field, size_t length, bool arrow) {
    Node *node          = ast_build_node(arena, NodeMember, location);
    node->member.object = object;
    node->member.field  = copy(arena, field, length);
    node->member.arrow  = arrow;
    return node;
}

Node *ast_build_cast_node(Arena *arena, Location location, Type *type, Node *operand) {
    Node *node         = ast_build_node(arena, NodeCast, location);
    node->cast.type    = type;
    node->cast.operand = operand;
    return node;
}

Node *ast_build_sizeof_node(Arena *arena, Location location, Type *type, Node *operand) {
    Node *node            = ast_build_node(arena, NodeSizeof, location);
    node->size_of.type    = type;
    node->size_of.operand = operand;
    return node;
}

Node *ast_build_expression_statement_node(Arena *arena, Location location, Node *expression) {
    Node *node                            = ast_build_node(arena, NodeExpressionStatement, location);
    node->expression_statement.expression = expression;
    return node;
}

Node *ast_build_compound_node(Arena *arena, Location location, Node *body) {
    Node *node          = ast_build_node(arena, NodeCompound, location);
    node->compound.body = body;
    return node;
}

Node *ast_build_if_node(Arena *arena, Location location, Node *condition, Node *then, Node *otherwise) {
    Node *node                   = ast_build_node(arena, NodeIf, location);
    node->if_statement.condition = condition;
    node->if_statement.then      = then;
    node->if_statement.otherwise = otherwise;
    return node;
}

Node *ast_build_while_node(Arena *arena, Location location, Node *condition, Node *body) {
    Node *node                      = ast_build_node(arena, NodeWhile, location);
    node->while_statement.condition = condition;
    node->while_statement.body      = body;
    return node;
}

Node *ast_build_for_node(Arena *arena, Location location, Node *initializer, Node *condition, Node *step, Node *body) {
    Node *node                      = ast_build_node(arena, NodeFor, location);
    node->for_statement.initializer = initializer;
    node->for_statement.condition   = condition;
    node->for_statement.step        = step;
    node->for_statement.body        = body;
    return node;
}

Node *ast_build_switch_node(Arena *arena, Location location, Node *expression, Node *body) {
    Node *node                        = ast_build_node(arena, NodeSwitch, location);
    node->switch_statement.expression = expression;
    node->switch_statement.body       = body;
    return node;
}

Node *ast_build_case_node(Arena *arena, Location location, Node *expression, Node *statement) {
    Node *node                      = ast_build_node(arena, NodeCase, location);
    node->case_statement.expression = expression;
    node->case_statement.statement  = statement;
    return node;
}

Node *ast_build_default_node(Arena *arena, Location location, Node *statement) {
    Node *node                        = ast_build_node(arena, NodeDefault, location);
    node->default_statement.statement = statement;
    return node;
}

Node *ast_build_break_node(Arena *arena, Location location) {
    return ast_build_node(arena, NodeBreak, location);
}

Node *ast_build_continue_node(Arena *arena, Location location) {
    return ast_build_node(arena, NodeContinue, location);
}

Node *ast_build_return_node(Arena *arena, Location location, Node *expression) {
    Node *node = ast_build_node(arena, NodeReturn, location);
    node->return_statement.expression = expression;
    return node;
}

Node *ast_build_variable_node(Arena *arena, Location location, Type *type, const char *name, size_t length, Node *initializer, bool external) {
    Node *node                 = ast_build_node(arena, NodeVariable, location);
    node->variable.type        = type;
    node->variable.name        = copy(arena, name, length);
    node->variable.initializer = initializer;
    node->variable.external    = external;
    return node;
}

Node *ast_build_function_node(Arena *arena, Location location, Type *type, const char *name, size_t length, Node *body, bool external) {
    Node *node              = ast_build_node(arena, NodeFunction, location);
    node->function.type     = type;
    node->function.name     = copy(arena, name, length);
    node->function.body     = body;
    node->function.external = external;
    return node;
}

Node *ast_build_typedef_node(Arena *arena, Location location, Type *type, const char *name, size_t length) {
    Node *node                 = ast_build_node(arena, NodeTypedef, location);
    node->type_definition.type = type;
    node->type_definition.name = copy(arena, name, length);
    return node;
}

Node *ast_build_struct_node(Arena *arena, Location location, Type *type) {
    Node *node     = ast_build_node(arena, NodeStruct, location);
    node->tag.type = type;
    return node;
}

Node *ast_build_union_node(Arena *arena, Location location, Type *type) {
    Node *node     = ast_build_node(arena, NodeUnion, location);
    node->tag.type = type;
    return node;
}

Node *ast_build_enum_node(Arena *arena, Location location, Type *type) {
    Node *node     = ast_build_node(arena, NodeEnum, location);
    node->tag.type = type;
    return node;
}

Node *ast_build_parameter_node(Arena *arena, Location location, const char *name, size_t length, Type *type) {
    Node *node           = ast_build_node(arena, NodeParameter, location);
    node->parameter.name = copy(arena, name, length);
    node->parameter.type = type;
    return node;
}

Node *ast_build_field_node(Arena *arena, Location location, const char *name, size_t length, Type *type, Node *width) {
    Node *node        = ast_build_node(arena, NodeField, location);
    node->field.name  = copy(arena, name, length);
    node->field.type  = type;
    node->field.width = width;
    return node;
}

Node *ast_build_enumerator_node(Arena *arena, Location location, const char *name, size_t length, Node *value) {
    Node *node             = ast_build_node(arena, NodeEnumerator, location);
    node->enumerator.name  = copy(arena, name, length);
    node->enumerator.value = value;
    return node;
}

Node *ast_build_field_designator_node(Arena *arena, Location location, const char *field, size_t length) {
    Node *node                   = ast_build_node(arena, NodeFieldDesignator, location);
    node->field_designator.field = copy(arena, field, length);
    return node;
}

Node *ast_build_index_designator_node(Arena *arena, Location location, Node *index) {
    Node *node                   = ast_build_node(arena, NodeIndexDesignator, location);
    node->index_designator.index = index;
    return node;
}

Node *ast_build_initializer_node(Arena *arena, Location location, Node *designator, Node *expression) {
    Node *node                   = ast_build_node(arena, NodeInitializer, location);
    node->initializer.designator = designator;
    node->initializer.expression = expression;
    return node;
}

Node *ast_build_initializer_list_node(Arena *arena, Location location, Node *designator, Node *items) {
    Node *node                        = ast_build_node(arena, NodeInitializerList, location);
    node->initializer_list.designator = designator;
    node->initializer_list.items      = items;
    return node;
}

Node *ast_build_translation_unit_node(Arena *arena, Location location, Node *declarations) {
    Node *node                          = ast_build_node(arena, NodeTranslationUnit, location);
    node->translation_unit.declarations = declarations;
    return node;
}
