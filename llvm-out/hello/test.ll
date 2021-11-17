; ModuleID = 'test.bc'
source_filename = "test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@.str.1 = private unnamed_addr constant [6 x i8] c"right\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i64, align 8
  store i32 0, i32* %1, align 4
  %3 = call i32 (i8*, ...) @__isoc99_scanf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str, i64 0, i64 0), i64* %2)
  %VarBasic = alloca i64
  %const = alloca i64
  %const2 = alloca i64
  %loads = load i64, i64* %2
  %s = add i64 %loads, 2001
  store i64 %s, i64* %VarBasic
  store i64 %loads, i64* %const2
  br label %loopEntry

loopEntry:                                        ; preds = %loopend, %0
  br label %cal1

cal1:                                             ; preds = %loopEntry
  %tmpvar1 = load i64, i64* %VarBasic
  %urem1 = srem i64 %tmpvar1, 2
  %4 = icmp eq i64 %urem1, 0
  br i1 %4, label %calt1, label %calf1

calt1:                                            ; preds = %cal1
  %tmpvar2 = load i64, i64* %VarBasic
  %div2 = udiv i64 %tmpvar2, 2
  store i64 %div2, i64* %VarBasic
  br label %checks

calf1:                                            ; preds = %cal1
  %tmpvar3 = load i64, i64* %VarBasic
  %mulp1 = mul i64 %tmpvar3, 3
  %addx = add i64 %mulp1, 1
  store i64 %addx, i64* %VarBasic
  br label %checks

checks:                                           ; preds = %calf1, %calt1
  %tmpVar4 = load i64, i64* %VarBasic
  %5 = icmp ne i64 %tmpVar4, 1
  br i1 %5, label %first, label %loopend

first:                                            ; preds = %checks
  %6 = load i64, i64* %2, align 8
  %7 = icmp eq i64 %6, 30
  br i1 %7, label %9, label %else

else:                                             ; preds = %first
  %a = load i64, i64* %const2
  %8 = icmp ne i64 %a, 30
  br i1 %8, label %11, label %loopend

9:                                                ; preds = %first
  %10 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.1, i64 0, i64 0))
  br label %11

loopend:                                          ; preds = %else, %checks
  br label %loopEntry

11:                                               ; preds = %else, %9
  ret i32 0
}

declare dso_local i32 @__isoc99_scanf(i8*, ...) #1

declare dso_local i32 @printf(i8*, ...) #1

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
