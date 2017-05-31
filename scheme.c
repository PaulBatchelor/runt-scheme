#include <stdio.h>
#include <runt.h>
#include <stdlib.h>
#include <string.h>

#include "rscheme.h"

#include "scheme-private.h"

#define car(p) ((p)->_object._cons._car)
#define cdr(p) ((p)->_object._cons._cdr)
#define FUNC(NAME, FUNC) scheme_define(sc, sc->global_env, \
        mk_symbol(sc, NAME), \
        mk_foreign_func(sc, FUNC)) 

static pointer scm_get_id(scheme *sc, pointer args)
{
    const char *str;
    runt_uint id;
    runt_entry *entry;
    runt_vm *vm;

    vm = sc->ext_data;
    str = string_value(car(args));
    if(runt_word_search(vm, str, strlen(str), &entry) != RUNT_OK) {
        runt_print(vm, "scheme: could not find runt function %s\n", str);
        runt_set_state(vm, RUNT_MODE_PANIC, RUNT_ON);
        return sc->NIL;
    }
    id = entry->cell->id;

    return mk_integer(sc, id);
}

static pointer scm_exec(scheme *sc, pointer args)
{
    runt_uint id;
    runt_vm *vm;
    pointer p;

    p = car(args);
    if(p == sc->NIL) {
        return sc->NIL;
    } else {
        vm = sc->ext_data;
        id = ivalue(car(args));
        runt_cell_id_exec(vm, id);
    }
    return sc->NIL;
}

static pointer scm_push(scheme *sc, pointer args)
{
    runt_vm *vm;
    runt_int rc;
    runt_stacklet *s;
    double val;

    vm = sc->ext_data;
    rc = runt_ppush(vm, &s);
    if(rc != RUNT_OK) {
        return sc->NIL;
    }
    val = rvalue(car(args));
    s->f = (runt_float)val;

    return sc->NIL;
}

static pointer scm_pop(scheme *sc, pointer args)
{
    runt_vm *vm;
    runt_int rc;
    runt_stacklet *s;

    vm = sc->ext_data;
    rc = runt_ppop(vm, &s);
    if(rc != RUNT_OK) {
        return sc->NIL;
    }

    return mk_real(sc, s->f);
}

static int rproc_scm_new(runt_vm *vm, runt_ptr p)
{
    scheme *sc;
    runt_int rc;
    runt_stacklet *s;

    rc = runt_ppush(vm, &s);
    RUNT_ERROR_CHECK(rc);

    sc = malloc(sizeof(scheme));
    scheme_init(sc);
    sc->code = sc->NIL;
    scheme_set_input_port_file(sc, stdin);
    scheme_set_output_port_file(sc, stdout);
    sc->ext_data = vm;

    FUNC("rnt-id", scm_get_id);
    FUNC("rnt-ex", scm_exec);
    FUNC("rnt-push", scm_push);
    FUNC("rnt-pop", scm_pop);
    s->p = runt_mk_cptr(vm, sc);
    return RUNT_OK;
}

static int rproc_scm_load(runt_vm *vm, runt_ptr p)
{
    scheme *sc;
    runt_int rc;
    runt_stacklet *s;
    const char *filename;
    FILE *fp;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    sc = runt_to_cptr(s->p);
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    filename = runt_to_string(s->p);

    fp = fopen(filename, "r");

    if(fp == NULL) {
        runt_print(vm, "Could not open file %s\n", filename);
        return RUNT_NOT_OK;
    }

    scheme_load_file(sc, fp);
    fclose(fp);
    return RUNT_OK;
}

static int rproc_scm_free(runt_vm *vm, runt_ptr p)
{
    scheme *sc;
    runt_int rc;
    runt_stacklet *s;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    sc = runt_to_cptr(s->p);

    scheme_load_string(sc, "(quit)");
    scheme_deinit(sc);

    free(sc);
    return RUNT_OK;
}

static int rproc_scm_eval(runt_vm *vm, runt_ptr p)
{
    scheme *sc;
    runt_int rc;
    runt_stacklet *s;
    const char *str;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    sc = runt_to_cptr(s->p);
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    str = runt_to_string(s->p);

    scheme_load_string(sc, str);

    return RUNT_OK;
}

int runt_load_scheme(runt_vm *vm)
{
    runt_word_define(vm, "scm_new", 7, rproc_scm_new);
    runt_word_define(vm, "scm_load", 8, rproc_scm_load);
    runt_word_define(vm, "scm_free", 8, rproc_scm_free);
    runt_word_define(vm, "scm_eval", 8, rproc_scm_eval);
    return runt_is_alive(vm);
}
