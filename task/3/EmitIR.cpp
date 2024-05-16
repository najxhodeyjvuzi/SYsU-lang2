#include "EmitIR.hpp"
#include <llvm/Transforms/Utils/ModuleUtils.h>

#define self (*this)

using namespace asg;

EmitIR::EmitIR(Obj::Mgr& mgr, llvm::LLVMContext& ctx, llvm::StringRef mid)
  : mMgr(mgr)
  , mMod(mid, ctx)
  , mCtx(ctx)
  , mIntTy(llvm::Type::getInt32Ty(ctx))
  , mCurIrb(std::make_unique<llvm::IRBuilder<>>(ctx))
  , mCtorTy(llvm::FunctionType::get(llvm::Type::getVoidTy(ctx), false))
  , mCurFunc(nullptr)
  /*
  在EmitIR类中，有以下成员变量：

  mMgr: Manager, 这是一个Obj::Mgr类型的引用，可能是用来管理一些对象或资源的管理器。

  mMod: Module, 这是一个llvm::Module对象，它在LLVM中表示一个编译单元，包含了函数和全局变量的定义。

  mCtx: Context, 这是一个llvm::LLVMContext引用，它在LLVM中表示一个独立的全局上下文。所有在同一个LLVMContext中创建的值都可以相互引用。

  mIntTy: Integer Type, 这是一个llvm::Type指针，它表示一个32位整型。在LLVM中，llvm::Type用来表示类型。

  mCurIrb: Current IR Builder, 这是一个llvm::IRBuilder的智能指针。llvm::IRBuilder是一个辅助类，用于生成LLVM指令。

  mCtorTy: Constructor Type, 这是一个llvm::FunctionType指针，它表示一个没有参数且返回类型为void的函数类型。

  这些成员变量在EmitIR的构造函数中被初始化。它们在后续的成员函数中被用来生成LLVM的中间表示（IR）。

  */
{
}

llvm::Module&
EmitIR::operator()(asg::TranslationUnit* tu)
{
  for (auto&& i : tu->decls)
    self(i);
  return mMod;
}

//==============================================================================
// 类型
//==============================================================================

llvm::Type*
EmitIR::operator()(const Type* type)
{
  if (type->texp == nullptr) {
    switch (type->spec) {
      case Type::Spec::kInt:
        return llvm::Type::getInt32Ty(mCtx);
      // TODO: 在此添加对更多基础类型的处理
      default:
        ABORT();
    }
  }

  Type subt;
  subt.spec = type->spec;
  subt.qual = type->qual;
  subt.texp = type->texp->sub;

  // TODO: 在此添加对指针类型、数组类型和函数类型的处理

  if (auto p = type->texp->dcst<FunctionType>()) {
    std::vector<llvm::Type*> pty;
    // TODO: 在此添加对函数参数类型的处理
    return llvm::FunctionType::get(self(&subt), std::move(pty), false);
  }

  ABORT();
}

//==============================================================================
// 表达式
//==============================================================================

llvm::Value*
EmitIR::operator()(Expr* obj)
{
  // TODO: 在此添加对更多表达式处理的跳转
  if (auto p = obj->dcst<IntegerLiteral>())
    return self(p);

  if (auto p = obj->dcst<BinaryExpr>())
    return self(p);
  
  if (auto p = obj->dcst<ImplicitCastExpr>())
    return self(p);
  
  if (auto p = obj->dcst<DeclRefExpr>())
    return self(p);
  
  if (auto p = obj->dcst<UnaryExpr>())
    return self(p);

  if (auto p = obj->dcst<ParenExpr>())
    return self(p);

  ABORT();
}

llvm::Constant*
EmitIR::operator()(IntegerLiteral* obj)
{
  return llvm::ConstantInt::get(self(obj->type), obj->val);
}

// TODO: 在此添加对更多表达式类型的处理

llvm::Value* EmitIR::operator()(ParenExpr* obj) {
  return self(obj->sub);
}

llvm::Value* EmitIR::operator()(UnaryExpr* obj) {
  auto sub = self(obj->sub);

  auto &irb = *mCurIrb;

  switch (obj->op) {
    case UnaryExpr::Op::kPos:
      return sub;
    case UnaryExpr::Op::kNeg:
      return irb.CreateNeg(sub);
    case UnaryExpr::Op::kNot:
      return irb.CreateNot(sub);
    default:
      ABORT();
  }

}

llvm::Value* EmitIR::operator()(BinaryExpr* obj) {
  auto& irb = *mCurIrb;

  auto lft = self(obj->lft);
  auto rht = self(obj->rht);

  switch (obj->op) {
    case BinaryExpr::Op::kAdd:
      return irb.CreateAdd(lft, rht);
    case BinaryExpr::Op::kSub:
      return irb.CreateSub(lft, rht);
    case BinaryExpr::Op::kMul:
      return irb.CreateMul(lft, rht);
    case BinaryExpr::Op::kDiv:
      return irb.CreateSDiv(lft, rht);
    case BinaryExpr::Op::kMod:
      return irb.CreateSRem(lft, rht);
    case BinaryExpr::Op::kEq:
      return irb.CreateICmpEQ(lft, rht);
    case BinaryExpr::Op::kNe:
      return irb.CreateICmpNE(lft, rht);
    case BinaryExpr::Op::kLt:
      return irb.CreateICmpSLT(lft, rht);
    case BinaryExpr::Op::kLe:
      return irb.CreateICmpSLE(lft, rht);
    case BinaryExpr::Op::kGt:
      return irb.CreateICmpSGT(lft, rht);
    case BinaryExpr::Op::kGe:
      return irb.CreateICmpSGE(lft, rht);
    case BinaryExpr::Op::kComma:
      return rht;
    case BinaryExpr::Op::kAssign:
      return irb.CreateStore(rht, lft);
    default:
      ABORT();
  }
}

llvm::Value* EmitIR::operator()(ImplicitCastExpr* obj) {
  auto sub = self(obj->sub);

  auto &irb = *mCurIrb;

  switch (obj->kind) {
    case ImplicitCastExpr::kLValueToRValue:{
      auto type = self(obj->sub->type);
      return irb.CreateLoad(type, sub);
    }
    case ImplicitCastExpr::kIntegralCast:{
      return irb.CreateIntCast(sub, self(obj->sub->type), true);
    }
    default:
      ABORT();
  }
}

llvm::Value* EmitIR::operator()(DeclRefExpr* obj) {
  return reinterpret_cast<llvm::Value*>(obj->decl->any);
}

//==============================================================================
// 语句
//==============================================================================

void
EmitIR::operator()(Stmt* obj)
{
  // TODO: 在此添加对更多Stmt类型的处理的跳转
  auto& irb = *mCurIrb;

  if (auto p = obj->dcst<CompoundStmt>()) {
    auto sp = irb.CreateIntrinsic(llvm::Intrinsic::stacksave, {}, {}, nullptr, "sp");
    return self(p);
    irb.CreateIntrinsic(llvm::Intrinsic::stackrestore, {}, {sp});
  }

  if (auto p = obj->dcst<ReturnStmt>())
    return self(p);

  if (auto p = obj->dcst<DeclStmt>())
    return self(p);

  if (auto p = obj->dcst<ExprStmt>())
    return self(p);

  ABORT();
}

// TODO: 在此添加对更多Stmt类型的处理

void
EmitIR::operator()(ExprStmt* obj)
{
  self(obj->expr);
}

void
EmitIR::operator()(CompoundStmt* obj)
{
  auto& irb = *mCurIrb;
  // TODO: 可以在此添加对符号重名的处理
  
  for (auto&& stmt : obj->subs)
    self(stmt);
}

void EmitIR::operator()(DeclStmt* obj) {
  for (auto&& decl : obj->decls)
    self(decl);
}

void
EmitIR::operator()(ReturnStmt* obj)
{
  auto& irb = *mCurIrb;

  llvm::Value* retVal;
  if (!obj->expr)
    retVal = nullptr;
  else
    retVal = self(obj->expr);

  mCurIrb->CreateRet(retVal);

  auto exitBb = llvm::BasicBlock::Create(mCtx, "return_exit", mCurFunc);
  mCurIrb = std::make_unique<llvm::IRBuilder<>>(exitBb);
}

//==============================================================================
// 声明
//==============================================================================

void EmitIR::transInit(llvm::Value* dst, Expr* src) {
  auto& irb = *mCurIrb;

  if (auto p = src->dcst<IntegerLiteral>()) {
    auto initVal = llvm::ConstantInt::get(self(p->type), p->val);
    irb.CreateStore(initVal, dst);
    return;
  }

  ABORT();
}

void EmitIR::operator()(Decl* obj) {
  if (auto p = obj->dcst<FunctionDecl>())
    return self(p);
  
  if (auto p = obj->dcst<VarDecl>())
    return self(p);
  
  ABORT();
}

void EmitIR::operator()(VarDecl* obj) {

  // 通过判断变量声明是否在基本块中，来判断变量是全局变量还是局部变量
  if (mCurFunc) {
    auto type = self(obj->type);
    auto alloca = mCurIrb->CreateAlloca(type, nullptr, obj->name);
    obj->any = alloca;

    if (obj->init == nullptr)
      return;
    
    transInit(alloca, obj->init);
    return;
  }

  // 生成全局变量

  auto preFunc = mCurFunc;

  auto type = llvm::Type::getInt64Ty(mCtx);

  auto gvar = new llvm::GlobalVariable(
    mMod, type, false, llvm::GlobalValue::ExternalLinkage, nullptr, obj->name);

  obj->any = gvar;

  gvar->setInitializer(llvm::ConstantInt::get(type, 0));

  if (obj->init == nullptr)
    return;
  
  // 生成构造函数
  // 生成构造函数的理由是：全局变量的初始化是在main函数之前的，而全局变量的初始化是在main函数之后的
  mCurFunc = llvm::Function::Create(
    mCtorTy, llvm::GlobalVariable::PrivateLinkage, obj->name + "ctor_" + obj->name, mMod);
  llvm::appendToGlobalCtors(mMod, mCurFunc, 65535);

  // 生成函数体  
  // 函数体的内容是：全局变量的初始化
  auto entryBb = llvm::BasicBlock::Create(mCtx, "entry", mCurFunc);
  // 生成IRBuilder
  // 为什么要以entryBb为参数呢？
  // 因为entryBb是构造函数的入口
  // IRBuilder的参数是一个BasicBlock，表示在这个BasicBlock中生成IR
  mCurIrb = std::make_unique<llvm::IRBuilder<>>(entryBb);
  // transinit的作用是：将一个表达式的值赋给一个变量
  transInit(gvar, obj->init);
  mCurIrb->CreateRet(nullptr);
  mCurFunc = preFunc;
  return;

}

void
EmitIR::operator()(FunctionDecl* obj)
{
  // 创建函数
  auto fty = llvm::dyn_cast<llvm::FunctionType>(self(obj->type));
  auto func = llvm::Function::Create(
    fty, llvm::GlobalVariable::ExternalLinkage, obj->name, mMod);

  obj->any = func;

  if (obj->body == nullptr)
    return;
  auto entryBb = llvm::BasicBlock::Create(mCtx, "entry", func);
  mCurIrb = std::make_unique<llvm::IRBuilder<>>(entryBb);
  auto& entryIrb = *mCurIrb;

  // TODO: 添加对函数参数的处理

  // 翻译函数体
  mCurFunc = func;
  self(obj->body);
  auto& exitIrb = *mCurIrb;

  if (fty->getReturnType()->isVoidTy())
    exitIrb.CreateRetVoid();
  else
    exitIrb.CreateUnreachable();
}
