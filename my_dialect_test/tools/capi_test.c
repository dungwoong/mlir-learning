#include "tens-c/Dialects.h"
#include "mlir-c/IR.h"
#include <stdio.h>

int main() {
  MlirContext ctx = mlirContextCreate();

  MlirDialectHandle tensHandle = mlirGetDialectHandle__tens__();
  mlirDialectHandleRegisterDialect(tensHandle, ctx);
  mlirDialectHandleLoadDialect(tensHandle, ctx);

  MlirStringRef ns = mlirDialectHandleGetNamespace(tensHandle);
  printf("loaded dialect namespace: %.*s\n", (int)ns.length, ns.data);
  printf("num registered dialects: %ld\n", mlirContextGetNumRegisteredDialects(ctx));

  mlirContextDestroy(ctx);
  return 0;
}