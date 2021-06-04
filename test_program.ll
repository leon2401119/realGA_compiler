; ModuleID = 'test_program.cpp'
source_filename = "test_program.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@_ZL4next = internal global i64 1, align 8
@.str = private unnamed_addr constant [21 x i8] c"Found duplicate: %d\0A\00", align 1

; Function Attrs: nounwind uwtable
define dso_local i32 @rand() #0 {
  %1 = load i64, i64* @_ZL4next, align 8, !tbaa !2
  %2 = mul i64 %1, 1103515245
  %3 = add i64 %2, 12345
  store i64 %3, i64* @_ZL4next, align 8, !tbaa !2
  %4 = load i64, i64* @_ZL4next, align 8, !tbaa !2
  %5 = udiv i64 %4, 65536
  %6 = trunc i64 %5 to i32
  %7 = urem i32 %6, 32767
  %8 = add i32 %7, 1
  ret i32 %8
}

; Function Attrs: nounwind uwtable
define dso_local void @srand(i32 %0) #0 {
  %2 = alloca i32, align 4
  store i32 %0, i32* %2, align 4, !tbaa !6
  %3 = load i32, i32* %2, align 4, !tbaa !6
  %4 = zext i32 %3 to i64
  store i64 %4, i64* @_ZL4next, align 8, !tbaa !2
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local i32 @_Z7randIntii(i32 %0, i32 %1) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store i32 %0, i32* %3, align 4, !tbaa !6
  store i32 %1, i32* %4, align 4, !tbaa !6
  %7 = bitcast i32* %5 to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %7) #5
  %8 = bitcast i32* %6 to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %8) #5
  %9 = load i32, i32* %4, align 4, !tbaa !6
  %10 = load i32, i32* %3, align 4, !tbaa !6
  %11 = sub nsw i32 %9, %10
  %12 = add nsw i32 %11, 1
  store i32 %12, i32* %6, align 4, !tbaa !6
  %13 = load i32, i32* %6, align 4, !tbaa !6
  %14 = sitofp i32 %13 to double
  %15 = call i32 @rand() #5
  %16 = sitofp i32 %15 to double
  %17 = fdiv double %16, 3.276800e+04
  %18 = fmul double %14, %17
  %19 = fptosi double %18 to i32
  store i32 %19, i32* %5, align 4, !tbaa !6
  %20 = load i32, i32* %5, align 4, !tbaa !6
  %21 = load i32, i32* %6, align 4, !tbaa !6
  %22 = icmp eq i32 %20, %21
  br i1 %22, label %23, label %28

23:                                               ; preds = %2
  %24 = load i32, i32* %5, align 4, !tbaa !6
  %25 = load i32, i32* %3, align 4, !tbaa !6
  %26 = add nsw i32 %24, %25
  %27 = sub nsw i32 %26, 1
  br label %32

28:                                               ; preds = %2
  %29 = load i32, i32* %5, align 4, !tbaa !6
  %30 = load i32, i32* %3, align 4, !tbaa !6
  %31 = add nsw i32 %29, %30
  br label %32

32:                                               ; preds = %28, %23
  %33 = phi i32 [ %27, %23 ], [ %31, %28 ]
  %34 = bitcast i32* %6 to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %34) #5
  %35 = bitcast i32* %5 to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %35) #5
  ret i32 %33
}

; Function Attrs: argmemonly nounwind willreturn
declare void @llvm.lifetime.start.p0i8(i64 immarg, i8* nocapture) #1

; Function Attrs: argmemonly nounwind willreturn
declare void @llvm.lifetime.end.p0i8(i64 immarg, i8* nocapture) #1

; Function Attrs: nounwind uwtable
define dso_local void @_Z7shufflePii(i32* %0, i32 %1) #0 {
  %3 = alloca i32*, align 8
  %4 = alloca i32, align 4
  %5 = alloca i64, align 8
  %6 = alloca i64, align 8
  %7 = alloca i64, align 8
  %8 = alloca i32, align 4
  store i32* %0, i32** %3, align 8, !tbaa !8
  store i32 %1, i32* %4, align 4, !tbaa !6
  %9 = bitcast i64* %5 to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %9) #5
  %10 = bitcast i64* %6 to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %10) #5
  %11 = bitcast i64* %7 to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %11) #5
  %12 = bitcast i32* %8 to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %12) #5
  %13 = load i32, i32* %4, align 4, !tbaa !6
  %14 = sub nsw i32 %13, 1
  %15 = sext i32 %14 to i64
  store i64 %15, i64* %7, align 8, !tbaa !10
  br label %16

16:                                               ; preds = %55, %2
  %17 = load i64, i64* %7, align 8, !tbaa !10
  %18 = icmp ugt i64 %17, 0
  br i1 %18, label %19, label %58

19:                                               ; preds = %16
  %20 = load i64, i64* %7, align 8, !tbaa !10
  %21 = add i64 %20, 1
  %22 = uitofp i64 %21 to double
  %23 = call i32 @rand() #5
  %24 = sitofp i32 %23 to double
  %25 = fdiv double %24, 3.276800e+04
  %26 = fmul double %22, %25
  %27 = fptosi double %26 to i32
  %28 = sext i32 %27 to i64
  store i64 %28, i64* %6, align 8, !tbaa !10
  %29 = load i64, i64* %6, align 8, !tbaa !10
  %30 = load i64, i64* %7, align 8, !tbaa !10
  %31 = add i64 %30, 1
  %32 = icmp eq i64 %29, %31
  br i1 %32, label %33, label %36

33:                                               ; preds = %19
  %34 = load i64, i64* %6, align 8, !tbaa !10
  %35 = sub i64 %34, 1
  br label %38

36:                                               ; preds = %19
  %37 = load i64, i64* %6, align 8, !tbaa !10
  br label %38

38:                                               ; preds = %36, %33
  %39 = phi i64 [ %35, %33 ], [ %37, %36 ]
  store i64 %39, i64* %5, align 8, !tbaa !10
  %40 = load i32*, i32** %3, align 8, !tbaa !8
  %41 = load i64, i64* %7, align 8, !tbaa !10
  %42 = getelementptr inbounds i32, i32* %40, i64 %41
  %43 = load i32, i32* %42, align 4, !tbaa !6
  store i32 %43, i32* %8, align 4, !tbaa !6
  %44 = load i32*, i32** %3, align 8, !tbaa !8
  %45 = load i64, i64* %5, align 8, !tbaa !10
  %46 = getelementptr inbounds i32, i32* %44, i64 %45
  %47 = load i32, i32* %46, align 4, !tbaa !6
  %48 = load i32*, i32** %3, align 8, !tbaa !8
  %49 = load i64, i64* %7, align 8, !tbaa !10
  %50 = getelementptr inbounds i32, i32* %48, i64 %49
  store i32 %47, i32* %50, align 4, !tbaa !6
  %51 = load i32, i32* %8, align 4, !tbaa !6
  %52 = load i32*, i32** %3, align 8, !tbaa !8
  %53 = load i64, i64* %5, align 8, !tbaa !10
  %54 = getelementptr inbounds i32, i32* %52, i64 %53
  store i32 %51, i32* %54, align 4, !tbaa !6
  br label %55

55:                                               ; preds = %38
  %56 = load i64, i64* %7, align 8, !tbaa !10
  %57 = add i64 %56, -1
  store i64 %57, i64* %7, align 8, !tbaa !10
  br label %16

58:                                               ; preds = %16
  %59 = bitcast i32* %8 to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %59) #5
  %60 = bitcast i64* %7 to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %60) #5
  %61 = bitcast i64* %6 to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %61) #5
  %62 = bitcast i64* %5 to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %62) #5
  ret void
}

; Function Attrs: nounwind uwtable
define dso_local i32* @_Z17createRandomArrayi(i32 %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32*, align 8
  store i32 %0, i32* %2, align 4, !tbaa !6
  %6 = bitcast i32* %3 to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %6) #5
  %7 = bitcast i32* %4 to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %7) #5
  %8 = bitcast i32** %5 to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %8) #5
  %9 = load i32, i32* %2, align 4, !tbaa !6
  %10 = add nsw i32 %9, 1
  store i32 %10, i32* %4, align 4, !tbaa !6
  %11 = load i32, i32* %4, align 4, !tbaa !6
  %12 = sext i32 %11 to i64
  %13 = mul i64 %12, 4
  %14 = call noalias i8* @malloc(i64 %13) #5
  %15 = bitcast i8* %14 to i32*
  store i32* %15, i32** %5, align 8, !tbaa !8
  store i32 0, i32* %3, align 4, !tbaa !6
  br label %16

16:                                               ; preds = %26, %1
  %17 = load i32, i32* %3, align 4, !tbaa !6
  %18 = load i32, i32* %4, align 4, !tbaa !6
  %19 = icmp slt i32 %17, %18
  br i1 %19, label %20, label %29

20:                                               ; preds = %16
  %21 = load i32, i32* %3, align 4, !tbaa !6
  %22 = load i32*, i32** %5, align 8, !tbaa !8
  %23 = load i32, i32* %3, align 4, !tbaa !6
  %24 = sext i32 %23 to i64
  %25 = getelementptr inbounds i32, i32* %22, i64 %24
  store i32 %21, i32* %25, align 4, !tbaa !6
  br label %26

26:                                               ; preds = %20
  %27 = load i32, i32* %3, align 4, !tbaa !6
  %28 = add nsw i32 %27, 1
  store i32 %28, i32* %3, align 4, !tbaa !6
  br label %16

29:                                               ; preds = %16
  %30 = load i32, i32* %2, align 4, !tbaa !6
  %31 = call i32 @_Z7randIntii(i32 1, i32 %30)
  %32 = load i32*, i32** %5, align 8, !tbaa !8
  %33 = getelementptr inbounds i32, i32* %32, i64 0
  store i32 %31, i32* %33, align 4, !tbaa !6
  %34 = load i32*, i32** %5, align 8, !tbaa !8
  %35 = load i32, i32* %4, align 4, !tbaa !6
  call void @_Z7shufflePii(i32* %34, i32 %35)
  %36 = load i32*, i32** %5, align 8, !tbaa !8
  %37 = bitcast i32** %5 to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %37) #5
  %38 = bitcast i32* %4 to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %38) #5
  %39 = bitcast i32* %3 to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %39) #5
  ret i32* %36
}

; Function Attrs: nounwind
declare dso_local noalias i8* @malloc(i64) #2

; Function Attrs: nounwind uwtable
define dso_local i32 @_Z13findDuplicatePii(i32* %0, i32 %1) #0 {
  %3 = alloca i32*, align 8
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store i32* %0, i32** %3, align 8, !tbaa !8
  store i32 %1, i32* %4, align 4, !tbaa !6
  %7 = bitcast i32* %5 to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %7) #5
  %8 = bitcast i32* %6 to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %8) #5
  store i32 0, i32* %6, align 4, !tbaa !6
  store i32 0, i32* %5, align 4, !tbaa !6
  br label %9

9:                                                ; preds = %24, %2
  %10 = load i32, i32* %5, align 4, !tbaa !6
  %11 = load i32, i32* %4, align 4, !tbaa !6
  %12 = icmp slt i32 %10, %11
  br i1 %12, label %13, label %27

13:                                               ; preds = %9
  %14 = load i32, i32* %6, align 4, !tbaa !6
  %15 = load i32, i32* %5, align 4, !tbaa !6
  %16 = add nsw i32 %15, 1
  %17 = xor i32 %14, %16
  %18 = load i32*, i32** %3, align 8, !tbaa !8
  %19 = load i32, i32* %5, align 4, !tbaa !6
  %20 = sext i32 %19 to i64
  %21 = getelementptr inbounds i32, i32* %18, i64 %20
  %22 = load i32, i32* %21, align 4, !tbaa !6
  %23 = xor i32 %17, %22
  store i32 %23, i32* %6, align 4, !tbaa !6
  br label %24

24:                                               ; preds = %13
  %25 = load i32, i32* %5, align 4, !tbaa !6
  %26 = add nsw i32 %25, 1
  store i32 %26, i32* %5, align 4, !tbaa !6
  br label %9

27:                                               ; preds = %9
  %28 = load i32, i32* %4, align 4, !tbaa !6
  %29 = load i32, i32* %6, align 4, !tbaa !6
  %30 = xor i32 %29, %28
  store i32 %30, i32* %6, align 4, !tbaa !6
  %31 = load i32, i32* %6, align 4, !tbaa !6
  %32 = bitcast i32* %6 to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %32) #5
  %33 = bitcast i32* %5 to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %33) #5
  ret i32 %31
}

; Function Attrs: norecurse uwtable
define dso_local i32 @main() #3 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32*, align 8
  store i32 0, i32* %1, align 4
  %6 = bitcast i32* %2 to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %6) #5
  %7 = bitcast i32* %3 to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %7) #5
  %8 = bitcast i32* %4 to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %8) #5
  %9 = bitcast i32** %5 to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %9) #5
  call void @srand(i32 1) #5
  store i32 0, i32* %2, align 4, !tbaa !6
  br label %10

10:                                               ; preds = %29, %0
  %11 = load i32, i32* %2, align 4, !tbaa !6
  %12 = icmp slt i32 %11, 5
  br i1 %12, label %13, label %32

13:                                               ; preds = %10
  %14 = call i32* @_Z17createRandomArrayi(i32 500000)
  store i32* %14, i32** %5, align 8, !tbaa !8
  store i32 0, i32* %3, align 4, !tbaa !6
  br label %15

15:                                               ; preds = %21, %13
  %16 = load i32, i32* %3, align 4, !tbaa !6
  %17 = icmp slt i32 %16, 200
  br i1 %17, label %18, label %24

18:                                               ; preds = %15
  %19 = load i32*, i32** %5, align 8, !tbaa !8
  %20 = call i32 @_Z13findDuplicatePii(i32* %19, i32 500001)
  store i32 %20, i32* %4, align 4, !tbaa !6
  br label %21

21:                                               ; preds = %18
  %22 = load i32, i32* %3, align 4, !tbaa !6
  %23 = add nsw i32 %22, 1
  store i32 %23, i32* %3, align 4, !tbaa !6
  br label %15

24:                                               ; preds = %15
  %25 = load i32*, i32** %5, align 8, !tbaa !8
  %26 = bitcast i32* %25 to i8*
  call void @free(i8* %26) #5
  %27 = load i32, i32* %4, align 4, !tbaa !6
  %28 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([21 x i8], [21 x i8]* @.str, i64 0, i64 0), i32 %27)
  br label %29

29:                                               ; preds = %24
  %30 = load i32, i32* %2, align 4, !tbaa !6
  %31 = add nsw i32 %30, 1
  store i32 %31, i32* %2, align 4, !tbaa !6
  br label %10

32:                                               ; preds = %10
  %33 = bitcast i32** %5 to i8*
  call void @llvm.lifetime.end.p0i8(i64 8, i8* %33) #5
  %34 = bitcast i32* %4 to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %34) #5
  %35 = bitcast i32* %3 to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %35) #5
  %36 = bitcast i32* %2 to i8*
  call void @llvm.lifetime.end.p0i8(i64 4, i8* %36) #5
  ret i32 0
}

; Function Attrs: nounwind
declare dso_local void @free(i8*) #2

declare dso_local i32 @printf(i8*, ...) #4

attributes #0 = { nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { argmemonly nounwind willreturn }
attributes #2 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { norecurse uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
!2 = !{!3, !3, i64 0}
!3 = !{!"long long", !4, i64 0}
!4 = !{!"omnipotent char", !5, i64 0}
!5 = !{!"Simple C++ TBAA"}
!6 = !{!7, !7, i64 0}
!7 = !{!"int", !4, i64 0}
!8 = !{!9, !9, i64 0}
!9 = !{!"any pointer", !4, i64 0}
!10 = !{!11, !11, i64 0}
!11 = !{!"long", !4, i64 0}
