#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "alloc.h"
#include "error.h"
#include "token.h"

typedef struct Node Node;
typedef struct Type Type;

typedef enum {
    /* Expressions */
    NodeInteger,             /* 123       */
    NodeCharacter,           /* 'a'       */
    NodeIdentifier,          /* name      */
    NodeUnary,               /* -x        */
    NodeBinary,              /* x + y     */
    NodeTernary,             /* c ? a : b */
    NodeCall,                /* f(a)      */
    NodeIndex,               /* a[i]      */
    NodeMember,              /* a.b       */
    NodeCast,                /* (t) x     */
    NodeSizeof,              /* sizeof    */

    /* Statements */
    NodeExpressionStatement, /* expr;     */
    NodeCompound,            /* { ... }   */
    NodeIf,                  /* if        */
    NodeWhile,               /* while     */
    NodeFor,                 /* for       */
    NodeSwitch,              /* switch    */
    NodeCase,                /* case      */
    NodeDefault,             /* default   */
    NodeBreak,               /* break     */
    NodeContinue,            /* continue  */
    NodeReturn,              /* return    */

    /* Declarations */
    NodeVariable,            /* int x     */
    NodeFunction,            /* int f()   */
    NodeTypedef,             /* typedef   */
    NodeStruct,              /* struct    */
    NodeUnion,               /* union     */
    NodeEnum,                /* enum      */
    NodeParameter,           /* int x     */
    NodeField,               /* int x     */
    NodeEnumerator,          /* A = 1     */
    NodeFieldDesignator,     /* .x        */
    NodeIndexDesignator,     /* [i]       */
    NodeInitializer,         /* 1         */
    NodeInitializerList,     /* { 1, 2 }  */
    NodeTranslationUnit,     /* file      */
} NodeKind;

typedef enum {
    TypeVoid,     /* void     */
    TypeBool,     /* _Bool    */
    TypeChar,     /* char     */
    TypeInt,      /* int      */
    TypePointer,  /* pointer  */
    TypeArray,    /* array    */
    TypeFunction, /* function */
    TypeStruct,   /* struct   */
    TypeUnion,    /* union    */
    TypeEnum,     /* enum     */
} TypeKind;

/**
 * Type - Represents a type.
 * @kind: The type kind.
 * @constant: True when the type is const-qualified.
 * @pointer.base: The pointed-to type of a TypePointer type.
 * @array.element: The element type of a TypeArray type.
 * @array.length: The length expression of a TypeArray type, or NULL when omitted.
 * @function.result: The result type of a TypeFunction type.
 * @function.parameters: The NodeParameter list of a TypeFunction type, or NULL when there are none.
 * @record.name: The tag of a TypeStruct or TypeUnion type, or NULL when omitted.
 * @record.fields: The NodeField list of a TypeStruct or TypeUnion type, or NULL when the type is incomplete.
 * @enumeration.name: The tag of a TypeEnum type, or NULL when omitted.
 * @enumeration.enumerators: The NodeEnumerator list of a TypeEnum type, or NULL when the type is incomplete.
 */
struct Type {
    TypeKind kind;
    bool     constant;
    union {
        struct {
            Type *base;
        } pointer;
        struct {
            Type *element;
            Node *length;
        } array;
        struct {
            Type *result;
            Node *parameters;
        } function;
        struct {
            const char *name;
            Node       *fields;
        } record;
        struct {
            const char *name;
            Node       *enumerators;
        } enumeration;
    };
};

/**
 * Node - Represents a syntax tree node.
 * @kind: The node kind.
 * @location: The source file location.
 * @next: The next node in a list, or NULL at the end.
 * @integer.value: The value of a NodeInteger node.
 * @character.value: The value of a NodeCharacter node.
 * @identifier.name: The name of a NodeIdentifier node.
 * @unary.op: The operator token kind of a NodeUnary node.
 * @unary.operand: The operand of a NodeUnary node.
 * @binary.op: The operator token kind of a NodeBinary node.
 * @binary.left: The left operand of a NodeBinary node.
 * @binary.right: The right operand of a NodeBinary node.
 * @ternary.condition: The condition of a NodeTernary node.
 * @ternary.then: The then branch of a NodeTernary node.
 * @ternary.otherwise: The else branch of a NodeTernary node.
 * @call.callee: The callee of a NodeCall node.
 * @call.arguments: The argument list of a NodeCall node, or NULL when there are none.
 * @subscript.object: The indexed object of a NodeIndex node.
 * @subscript.index: The index expression of a NodeIndex node.
 * @member.object: The object of a NodeMember node.
 * @member.field: The field name of a NodeMember node.
 * @member.arrow: True when a NodeMember node uses ->.
 * @cast.type: The target type of a NodeCast node.
 * @cast.operand: The operand of a NodeCast node.
 * @size_of.type: The type operand of a NodeSizeof node, or NULL when the operand is an expression.
 * @size_of.operand: The expression operand of a NodeSizeof node, or NULL when the operand is a type.
 * @expression_statement.expression: The expression of a NodeExpressionStatement node, or NULL for an empty statement.
 * @compound.body: The block item list of a NodeCompound node, or NULL when the block is empty.
 * @if_statement.condition: The condition of a NodeIf node.
 * @if_statement.then: The then branch of a NodeIf node.
 * @if_statement.otherwise: The else branch of a NodeIf node, or NULL when omitted.
 * @while_statement.condition: The condition of a NodeWhile node.
 * @while_statement.body: The body of a NodeWhile node.
 * @for_statement.initializer: The initializer expression or declaration list of a NodeFor node, or NULL when omitted.
 * @for_statement.condition: The condition of a NodeFor node, or NULL when omitted.
 * @for_statement.step: The step expression of a NodeFor node, or NULL when omitted.
 * @for_statement.body: The body of a NodeFor node.
 * @switch_statement.expression: The expression of a NodeSwitch node.
 * @switch_statement.body: The body of a NodeSwitch node.
 * @case_statement.expression: The label expression of a NodeCase node.
 * @case_statement.statement: The labeled statement of a NodeCase node.
 * @default_statement.statement: The labeled statement of a NodeDefault node.
 * @return_statement.expression: The returned expression of a NodeReturn node, or NULL when omitted.
 * @variable.type: The type of a NodeVariable node.
 * @variable.name: The name of a NodeVariable node.
 * @variable.initializer: The initializer of a NodeVariable node, or NULL when omitted.
 * @variable.external: True when a NodeVariable node is extern.
 * @function.type: The type of a NodeFunction node.
 * @function.name: The name of a NodeFunction node.
 * @function.body: The body of a NodeFunction node, or NULL for a declaration.
 * @function.external: True when a NodeFunction node is extern.
 * @type_definition.type: The type of a NodeTypedef node.
 * @type_definition.name: The name of a NodeTypedef node.
 * @tag.type: The type of a NodeStruct, NodeUnion, or NodeEnum node.
 * @parameter.name: The name of a NodeParameter node, or NULL when omitted.
 * @parameter.type: The type of a NodeParameter node.
 * @field.name: The name of a NodeField node, or NULL when omitted.
 * @field.type: The type of a NodeField node.
 * @field.width: The bit-field width of a NodeField node, or NULL when omitted.
 * @enumerator.name: The name of a NodeEnumerator node.
 * @enumerator.value: The value of a NodeEnumerator node, or NULL when omitted.
 * @field_designator.field: The field name of a NodeFieldDesignator node.
 * @index_designator.index: The index expression of a NodeIndexDesignator node.
 * @initializer.designator: The designator list of a NodeInitializer node, or NULL when omitted.
 * @initializer.expression: The expression of a NodeInitializer node.
 * @initializer_list.designator: The designator list of a NodeInitializerList node, or NULL when omitted.
 * @initializer_list.items: The item list of a NodeInitializerList node, or NULL when the list is empty.
 * @translation_unit.declarations: The declaration list of a NodeTranslationUnit node, or NULL when the file is empty.
 */
struct Node {
    NodeKind kind;
    Location location;
    Node    *next;
    union {
        struct {
            unsigned long long value;
        } integer;
        struct {
            int value;
        } character;
        struct {
            const char *name;
        } identifier;
        struct {
            TokenKind  op;
            Node      *operand;
        } unary;
        struct {
            TokenKind  op;
            Node      *left;
            Node      *right;
        } binary;
        struct {
            Node *condition;
            Node *then;
            Node *otherwise;
        } ternary;
        struct {
            Node *callee;
            Node *arguments;
        } call;
        struct {
            Node *object;
            Node *index;
        } subscript;
        struct {
            Node       *object;
            const char *field;
            bool        arrow;
        } member;
        struct {
            Type *type;
            Node *operand;
        } cast;
        struct {
            Type *type;
            Node *operand;
        } size_of;
        struct {
            Node *expression;
        } expression_statement;
        struct {
            Node *body;
        } compound;
        struct {
            Node *condition;
            Node *then;
            Node *otherwise;
        } if_statement;
        struct {
            Node *condition;
            Node *body;
        } while_statement;
        struct {
            Node *initializer;
            Node *condition;
            Node *step;
            Node *body;
        } for_statement;
        struct {
            Node *expression;
            Node *body;
        } switch_statement;
        struct {
            Node *expression;
            Node *statement;
        } case_statement;
        struct {
            Node *statement;
        } default_statement;
        struct {
            Node *expression;
        } return_statement;
        struct {
            Type       *type;
            const char *name;
            Node       *initializer;
            bool        external;
        } variable;
        struct {
            Type       *type;
            const char *name;
            Node       *body;
            bool        external;
        } function;
        struct {
            Type       *type;
            const char *name;
        } type_definition;
        struct {
            Type *type;
        } tag;
        struct {
            const char *name;
            Type       *type;
        } parameter;
        struct {
            const char *name;
            Type       *type;
            Node       *width;
        } field;
        struct {
            const char *name;
            Node       *value;
        } enumerator;
        struct {
            const char *field;
        } field_designator;
        struct {
            Node *index;
        } index_designator;
        struct {
            Node *designator;
            Node *expression;
        } initializer;
        struct {
            Node *designator;
            Node *items;
        } initializer_list;
        struct {
            Node *declarations;
        } translation_unit;
    };
};

/**
 * ast_build_void_type() - Builds a void type.
 * @arena: The arena to allocate from.
 * @constant: True when the type is const-qualified.
 * Returns: The void type.
 */
Type *ast_build_void_type(Arena *arena, bool constant);

/**
 * ast_build_bool_type() - Builds a _Bool type.
 * @arena: The arena to allocate from.
 * @constant: True when the type is const-qualified.
 * Returns: The _Bool type.
 */
Type *ast_build_bool_type(Arena *arena, bool constant);

/**
 * ast_build_char_type() - Builds a char type.
 * @arena: The arena to allocate from.
 * @constant: True when the type is const-qualified.
 * Returns: The char type.
 */
Type *ast_build_char_type(Arena *arena, bool constant);

/**
 * ast_build_int_type() - Builds an int type.
 * @arena: The arena to allocate from.
 * @constant: True when the type is const-qualified.
 * Returns: The int type.
 */
Type *ast_build_int_type(Arena *arena, bool constant);

/**
 * ast_build_pointer_type() - Builds a pointer type.
 * @arena: The arena to allocate from.
 * @base: The pointed-to type.
 * @constant: True when the pointer is const-qualified.
 * Returns: The pointer type.
 */
Type *ast_build_pointer_type(Arena *arena, Type *base, bool constant);

/**
 * ast_build_array_type() - Builds an array type.
 * @arena: The arena to allocate from.
 * @element: The element type.
 * @length: The length expression, or NULL when omitted.
 * @constant: True when the array is const-qualified.
 * Returns: The array type.
 */
Type *ast_build_array_type(Arena *arena, Type *element, Node *length, bool constant);

/**
 * ast_build_function_type() - Builds a function type.
 * @arena: The arena to allocate from.
 * @result: The result type.
 * @parameters: The NodeParameter list, or NULL when there are none.
 * Returns: The function type.
 */
Type *ast_build_function_type(Arena *arena, Type *result, Node *parameters);

/**
 * ast_build_struct_type() - Builds a struct type.
 * @arena: The arena to allocate from.
 * @name: The tag, or NULL when omitted. It is copied into @arena.
 * @length: The number of bytes in @name.
 * @fields: The NodeField list, or NULL when the type is incomplete.
 * @constant: True when the type is const-qualified.
 * Returns: The struct type.
 */
Type *ast_build_struct_type(Arena *arena, const char *name, size_t length, Node *fields, bool constant);

/**
 * ast_build_union_type() - Builds a union type.
 * @arena: The arena to allocate from.
 * @name: The tag, or NULL when omitted. It is copied into @arena.
 * @length: The number of bytes in @name.
 * @fields: The NodeField list, or NULL when the type is incomplete.
 * @constant: True when the type is const-qualified.
 * Returns: The union type.
 */
Type *ast_build_union_type(Arena *arena, const char *name, size_t length, Node *fields, bool constant);

/**
 * ast_build_enum_type() - Builds an enum type.
 * @arena: The arena to allocate from.
 * @name: The tag, or NULL when omitted. It is copied into @arena.
 * @length: The number of bytes in @name.
 * @enumerators: The NodeEnumerator list, or NULL when the type is incomplete.
 * @constant: True when the type is const-qualified.
 * Returns: The enum type.
 */
Type *ast_build_enum_type(Arena *arena, const char *name, size_t length, Node *enumerators, bool constant);

/**
 * ast_build_integer_node() - Builds an integer node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @value: The integer value.
 * Returns: The integer node.
 */
Node *ast_build_integer_node(Arena *arena, Location location, unsigned long long value);

/**
 * ast_build_character_node() - Builds a character node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @value: The character value.
 * Returns: The character node.
 */
Node *ast_build_character_node(Arena *arena, Location location, int value);

/**
 * ast_build_identifier_node() - Builds an identifier node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @name: The identifier name. It is copied into @arena.
 * @length: The number of bytes in @name.
 * Returns: The identifier node.
 */
Node *ast_build_identifier_node(Arena *arena, Location location, const char *name, size_t length);

/**
 * ast_build_unary_node() - Builds a unary expression node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @op: The operator token kind.
 * @operand: The operand.
 * Returns: The unary expression node.
 */
Node *ast_build_unary_node(Arena *arena, Location location, TokenKind op, Node *operand);

/**
 * ast_build_binary_node() - Builds a binary expression node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @op: The operator token kind.
 * @left: The left operand.
 * @right: The right operand.
 * Returns: The binary expression node.
 */
Node *ast_build_binary_node(Arena *arena, Location location, TokenKind op, Node *left, Node *right);

/**
 * ast_build_ternary_node() - Builds a ternary expression node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @condition: The condition.
 * @then: The then branch.
 * @otherwise: The else branch.
 * Returns: The ternary expression node.
 */
Node *ast_build_ternary_node(Arena *arena, Location location, Node *condition, Node *then, Node *otherwise);

/**
 * ast_build_call_node() - Builds a call node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @callee: The callee.
 * @arguments: The argument list, or NULL when there are none.
 * Returns: The call node.
 */
Node *ast_build_call_node(Arena *arena, Location location, Node *callee, Node *arguments);

/**
 * ast_build_index_node() - Builds an index node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @object: The indexed object.
 * @index: The index expression.
 * Returns: The index node.
 */
Node *ast_build_index_node(Arena *arena, Location location, Node *object, Node *index);

/**
 * ast_build_member_node() - Builds a member node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @object: The object.
 * @field: The field name. It is copied into @arena.
 * @length: The number of bytes in @field.
 * @arrow: True when the operator is ->.
 * Returns: The member node.
 */
Node *ast_build_member_node(Arena *arena, Location location, Node *object, const char *field, size_t length, bool arrow);

/**
 * ast_build_cast_node() - Builds a cast node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @type: The target type.
 * @operand: The operand.
 * Returns: The cast node.
 */
Node *ast_build_cast_node(Arena *arena, Location location, Type *type, Node *operand);

/**
 * ast_build_sizeof_node() - Builds a sizeof node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @type: The type operand, or NULL when the operand is an expression.
 * @operand: The expression operand, or NULL when the operand is a type.
 * Returns: The sizeof node.
 */
Node *ast_build_sizeof_node(Arena *arena, Location location, Type *type, Node *operand);

/**
 * ast_build_expression_statement_node() - Builds an expression statement node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @expression: The expression, or NULL for an empty statement.
 * Returns: The expression statement node.
 */
Node *ast_build_expression_statement_node(Arena *arena, Location location, Node *expression);

/**
 * ast_build_compound_node() - Builds a compound statement node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @body: The block item list, or NULL when the block is empty.
 * Returns: The compound statement node.
 */
Node *ast_build_compound_node(Arena *arena, Location location, Node *body);

/**
 * ast_build_if_node() - Builds an if node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @condition: The condition.
 * @then: The then branch.
 * @otherwise: The else branch, or NULL when omitted.
 * Returns: The if node.
 */
Node *ast_build_if_node(Arena *arena, Location location, Node *condition, Node *then, Node *otherwise);

/**
 * ast_build_while_node() - Builds a while node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @condition: The condition.
 * @body: The body.
 * Returns: The while node.
 */
Node *ast_build_while_node(Arena *arena, Location location, Node *condition, Node *body);

/**
 * ast_build_for_node() - Builds a for node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @initializer: The initializer expression or declaration list, or NULL when omitted.
 * @condition: The condition, or NULL when omitted.
 * @step: The step expression, or NULL when omitted.
 * @body: The body.
 * Returns: The for node.
 */
Node *ast_build_for_node(Arena *arena, Location location, Node *initializer, Node *condition, Node *step, Node *body);

/**
 * ast_build_switch_node() - Builds a switch node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @expression: The expression.
 * @body: The body.
 * Returns: The switch node.
 */
Node *ast_build_switch_node(Arena *arena, Location location, Node *expression, Node *body);

/**
 * ast_build_case_node() - Builds a case node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @expression: The label expression.
 * @statement: The labeled statement.
 * Returns: The case node.
 */
Node *ast_build_case_node(Arena *arena, Location location, Node *expression, Node *statement);

/**
 * ast_build_default_node() - Builds a default node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @statement: The labeled statement.
 * Returns: The default node.
 */
Node *ast_build_default_node(Arena *arena, Location location, Node *statement);

/**
 * ast_build_break_node() - Builds a break node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * Returns: The break node.
 */
Node *ast_build_break_node(Arena *arena, Location location);

/**
 * ast_build_continue_node() - Builds a continue node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * Returns: The continue node.
 */
Node *ast_build_continue_node(Arena *arena, Location location);

/**
 * ast_build_return_node() - Builds a return node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @expression: The returned expression, or NULL when omitted.
 * Returns: The return node.
 */
Node *ast_build_return_node(Arena *arena, Location location, Node *expression);

/**
 * ast_build_variable_node() - Builds a variable node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @type: The variable type.
 * @name: The variable name. It is copied into @arena.
 * @length: The number of bytes in @name.
 * @initializer: The initializer, or NULL when omitted.
 * @external: True when the declaration is extern.
 * Returns: The variable node.
 */
Node *ast_build_variable_node(Arena *arena, Location location, Type *type, const char *name, size_t length, Node *initializer, bool external);

/**
 * ast_build_function_node() - Builds a function node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @type: The function type.
 * @name: The function name. It is copied into @arena.
 * @length: The number of bytes in @name.
 * @body: The function body, or NULL for a declaration.
 * @external: True when the declaration is extern.
 * Returns: The function node.
 */
Node *ast_build_function_node(Arena *arena, Location location, Type *type, const char *name, size_t length, Node *body, bool external);

/**
 * ast_build_typedef_node() - Builds a typedef node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @type: The defined type.
 * @name: The typedef name. It is copied into @arena.
 * @length: The number of bytes in @name.
 * Returns: The typedef node.
 */
Node *ast_build_typedef_node(Arena *arena, Location location, Type *type, const char *name, size_t length);

/**
 * ast_build_struct_node() - Builds a struct declaration node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @type: The struct type.
 * Returns: The struct declaration node.
 */
Node *ast_build_struct_node(Arena *arena, Location location, Type *type);

/**
 * ast_build_union_node() - Builds a union declaration node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @type: The union type.
 * Returns: The union declaration node.
 */
Node *ast_build_union_node(Arena *arena, Location location, Type *type);

/**
 * ast_build_enum_node() - Builds an enum declaration node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @type: The enum type.
 * Returns: The enum declaration node.
 */
Node *ast_build_enum_node(Arena *arena, Location location, Type *type);

/**
 * ast_build_parameter_node() - Builds a parameter node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @name: The parameter name, or NULL when omitted. It is copied into @arena.
 * @length: The number of bytes in @name.
 * @type: The parameter type.
 * Returns: The parameter node.
 */
Node *ast_build_parameter_node(Arena *arena, Location location, const char *name, size_t length, Type *type);

/**
 * ast_build_field_node() - Builds a field node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @name: The field name, or NULL when omitted. It is copied into @arena.
 * @length: The number of bytes in @name.
 * @type: The field type.
 * @width: The bit-field width, or NULL when omitted.
 * Returns: The field node.
 */
Node *ast_build_field_node(Arena *arena, Location location, const char *name, size_t length, Type *type, Node *width);

/**
 * ast_build_enumerator_node() - Builds an enumerator node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @name: The enumerator name. It is copied into @arena.
 * @length: The number of bytes in @name.
 * @value: The enumerator value, or NULL when omitted.
 * Returns: The enumerator node.
 */
Node *ast_build_enumerator_node(Arena *arena, Location location, const char *name, size_t length, Node *value);

/**
 * ast_build_field_designator_node() - Builds a field designator node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @field: The field name. It is copied into @arena.
 * @length: The number of bytes in @field.
 * Returns: The field designator node.
 */
Node *ast_build_field_designator_node(Arena *arena, Location location, const char *field, size_t length);

/**
 * ast_build_index_designator_node() - Builds an index designator node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @index: The index expression.
 * Returns: The index designator node.
 */
Node *ast_build_index_designator_node(Arena *arena, Location location, Node *index);

/**
 * ast_build_initializer_node() - Builds an initializer node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @designator: The designator list, or NULL when omitted.
 * @expression: The initializer expression.
 * Returns: The initializer node.
 */
Node *ast_build_initializer_node(Arena *arena, Location location, Node *designator, Node *expression);

/**
 * ast_build_initializer_list_node() - Builds an initializer list node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @designator: The designator list, or NULL when omitted.
 * @items: The item list, or NULL when the list is empty.
 * Returns: The initializer list node.
 */
Node *ast_build_initializer_list_node(Arena *arena, Location location, Node *designator, Node *items);

/**
 * ast_build_translation_unit_node() - Builds a translation unit node.
 * @arena: The arena to allocate from.
 * @location: The source file location.
 * @declarations: The declaration list, or NULL when the file is empty.
 * Returns: The translation unit node.
 */
Node *ast_build_translation_unit_node(Arena *arena, Location location, Node *declarations);
