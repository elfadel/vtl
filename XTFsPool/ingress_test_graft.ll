; ModuleID = 'ingress_test_graft.c'
source_filename = "ingress_test_graft.c"
target datalayout = "e-m:e-p:64:64-i64:64-n32:64-S128"
target triple = "bpf"

%struct.bpf_map_def = type { i32, i32, i32, i32, i32, i32, i32 }
%struct.xdp_md = type { i32, i32, i32, i32, i32 }
%struct.ethhdr = type { [6 x i8], [6 x i8], i16 }
%struct.iphdr = type { i8, i8, i16, i16, i16, i8, i8, i16, i32, i32 }
%struct.vtl_hdr_t = type { i8, i32, i64, i16, i16 }

@xsks_map = global %struct.bpf_map_def { i32 17, i32 4, i32 4, i32 64, i32 0, i32 0, i32 0 }, section "maps", align 4, !dbg !0
@NUM_ACK_MAP = global %struct.bpf_map_def { i32 2, i32 4, i32 2, i32 1, i32 0, i32 0, i32 0 }, section "maps", align 4, !dbg !85
@_license = global [4 x i8] c"GPL\00", section "license", align 1, !dbg !97
@vtl_csum.____fmt = private unnamed_addr constant [35 x i8] c"vtl_csum(): malformed ETH header.\0A\00", align 1
@vtl_csum.____fmt.1 = private unnamed_addr constant [34 x i8] c"vtl_csum(): malformed IP header.\0A\00", align 1
@vtl_csum.____fmt.2 = private unnamed_addr constant [35 x i8] c"vtl_csum(): malformed VTL header.\0A\00", align 1
@vtl_csum.____fmt.3 = private unnamed_addr constant [31 x i8] c"VTL layer: malformed payload.\0A\00", align 1
@llvm.used = appending global [4 x i8*] [i8* bitcast (%struct.bpf_map_def* @NUM_ACK_MAP to i8*), i8* getelementptr inbounds ([4 x i8], [4 x i8]* @_license, i32 0, i32 0), i8* bitcast (i32 (%struct.xdp_md*)* @xdp_sock_prog to i8*), i8* bitcast (%struct.bpf_map_def* @xsks_map to i8*)], section "llvm.metadata"

; Function Attrs: nounwind
define i32 @xdp_sock_prog(%struct.xdp_md*) #0 section "ingress_tf_sec" !dbg !136 {
  %2 = alloca [35 x i8], align 1
  call void @llvm.dbg.declare(metadata [35 x i8]* %2, metadata !164, metadata !DIExpression()), !dbg !206
  %3 = alloca [34 x i8], align 1
  call void @llvm.dbg.declare(metadata [34 x i8]* %3, metadata !179, metadata !DIExpression()), !dbg !209
  %4 = alloca [35 x i8], align 1
  call void @llvm.dbg.declare(metadata [35 x i8]* %4, metadata !187, metadata !DIExpression()), !dbg !210
  %5 = alloca [31 x i8], align 1
  call void @llvm.dbg.declare(metadata [31 x i8]* %5, metadata !195, metadata !DIExpression()), !dbg !211
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  call void @llvm.dbg.value(metadata %struct.xdp_md* %0, metadata !149, metadata !DIExpression()), !dbg !212
  %8 = bitcast i32* %6 to i8*, !dbg !213
  call void @llvm.lifetime.start.p0i8(i64 4, i8* nonnull %8) #3, !dbg !213
  call void @llvm.dbg.value(metadata i32 0, metadata !150, metadata !DIExpression()), !dbg !214
  store i32 0, i32* %6, align 4, !dbg !214, !tbaa !215
  %9 = bitcast i32* %7 to i8*, !dbg !213
  call void @llvm.lifetime.start.p0i8(i64 4, i8* nonnull %9) #3, !dbg !213
  %10 = getelementptr inbounds %struct.xdp_md, %struct.xdp_md* %0, i64 0, i32 4, !dbg !219
  %11 = load i32, i32* %10, align 4, !dbg !219, !tbaa !220
  call void @llvm.dbg.value(metadata i32 %11, metadata !151, metadata !DIExpression()), !dbg !222
  store i32 %11, i32* %7, align 4, !dbg !222, !tbaa !215
  call void @llvm.dbg.value(metadata i16* null, metadata !152, metadata !DIExpression()), !dbg !223
  %12 = getelementptr inbounds %struct.xdp_md, %struct.xdp_md* %0, i64 0, i32 0, !dbg !224
  %13 = load i32, i32* %12, align 4, !dbg !224, !tbaa !225
  %14 = zext i32 %13 to i64, !dbg !226
  %15 = inttoptr i64 %14 to i8*, !dbg !227
  call void @llvm.dbg.value(metadata i8* %15, metadata !153, metadata !DIExpression()), !dbg !228
  %16 = getelementptr inbounds %struct.xdp_md, %struct.xdp_md* %0, i64 0, i32 1, !dbg !229
  %17 = load i32, i32* %16, align 4, !dbg !229, !tbaa !230
  %18 = zext i32 %17 to i64, !dbg !231
  %19 = inttoptr i64 %18 to i8*, !dbg !232
  call void @llvm.dbg.value(metadata i8* %19, metadata !154, metadata !DIExpression()), !dbg !233
  %20 = inttoptr i64 %14 to %struct.ethhdr*, !dbg !234
  call void @llvm.dbg.value(metadata %struct.ethhdr* %20, metadata !155, metadata !DIExpression()), !dbg !235
  %21 = getelementptr inbounds %struct.ethhdr, %struct.ethhdr* %20, i64 1, !dbg !236
  %22 = inttoptr i64 %18 to %struct.ethhdr*, !dbg !238
  %23 = icmp ugt %struct.ethhdr* %21, %22, !dbg !239
  br i1 %23, label %147, label %24, !dbg !240

; <label>:24:                                     ; preds = %1
  call void @llvm.dbg.value(metadata %struct.ethhdr* %21, metadata !156, metadata !DIExpression()), !dbg !241
  %25 = getelementptr inbounds %struct.ethhdr, %struct.ethhdr* %20, i64 2, i32 1, !dbg !242
  %26 = bitcast [6 x i8]* %25 to %struct.iphdr*, !dbg !242
  %27 = inttoptr i64 %18 to %struct.iphdr*, !dbg !244
  %28 = icmp ugt %struct.iphdr* %26, %27, !dbg !245
  br i1 %28, label %147, label %29, !dbg !246

; <label>:29:                                     ; preds = %24
  %30 = getelementptr inbounds %struct.ethhdr, %struct.ethhdr* %20, i64 1, i32 1, i64 3, !dbg !247
  %31 = load i8, i8* %30, align 1, !dbg !247, !tbaa !249
  %32 = icmp eq i8 %31, -3, !dbg !252
  br i1 %32, label %33, label %147, !dbg !253

; <label>:33:                                     ; preds = %29
  call void @llvm.dbg.value(metadata [6 x i8]* %25, metadata !157, metadata !DIExpression()), !dbg !254
  %34 = getelementptr inbounds [6 x i8], [6 x i8]* %25, i64 4, !dbg !255
  %35 = bitcast [6 x i8]* %34 to %struct.vtl_hdr_t*, !dbg !255
  %36 = inttoptr i64 %18 to %struct.vtl_hdr_t*, !dbg !257
  %37 = icmp ugt %struct.vtl_hdr_t* %35, %36, !dbg !258
  br i1 %37, label %147, label %38, !dbg !259

; <label>:38:                                     ; preds = %33
  %39 = call i8* inttoptr (i64 1 to i8* (i8*, i8*)*)(i8* bitcast (%struct.bpf_map_def* @NUM_ACK_MAP to i8*), i8* nonnull %8) #3, !dbg !260
  %40 = bitcast i8* %39 to i16*, !dbg !261
  call void @llvm.dbg.value(metadata i16* %40, metadata !152, metadata !DIExpression()), !dbg !223
  %41 = icmp eq i8* %39, null, !dbg !262
  br i1 %41, label %147, label %42, !dbg !264

; <label>:42:                                     ; preds = %38
  %43 = getelementptr inbounds [6 x i8], [6 x i8]* %25, i64 2, i64 4, !dbg !265
  %44 = bitcast i8* %43 to i16*, !dbg !265
  %45 = load i16, i16* %44, align 8, !dbg !265, !tbaa !266
  call void @llvm.dbg.value(metadata i16 %45, metadata !158, metadata !DIExpression()), !dbg !269
  call void @llvm.dbg.value(metadata i16 0, metadata !159, metadata !DIExpression()), !dbg !270
  call void @llvm.dbg.value(metadata i8* %15, metadata !172, metadata !DIExpression()) #3, !dbg !271
  call void @llvm.dbg.value(metadata i8* %19, metadata !173, metadata !DIExpression()) #3, !dbg !272
  call void @llvm.dbg.value(metadata i32 0, metadata !175, metadata !DIExpression()) #3, !dbg !273
  call void @llvm.dbg.value(metadata i8 0, metadata !176, metadata !DIExpression()) #3, !dbg !274
  call void @llvm.dbg.value(metadata i8* %15, metadata !177, metadata !DIExpression()) #3, !dbg !275
  %46 = getelementptr inbounds i8, i8* %15, i64 14, !dbg !276
  %47 = icmp ugt i8* %46, %19, !dbg !277
  br i1 %47, label %48, label %51, !dbg !278

; <label>:48:                                     ; preds = %42
  %49 = getelementptr inbounds [35 x i8], [35 x i8]* %2, i64 0, i64 0, !dbg !206
  call void @llvm.lifetime.start.p0i8(i64 35, i8* nonnull %49) #3, !dbg !206
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* nonnull %49, i8* getelementptr inbounds ([35 x i8], [35 x i8]* @vtl_csum.____fmt, i64 0, i64 0), i64 35, i32 1, i1 false) #3, !dbg !206
  %50 = call i32 (i8*, i32, ...) inttoptr (i64 7 to i32 (i8*, i32, ...)*)(i8* nonnull %49, i32 35) #3, !dbg !206
  call void @llvm.lifetime.end.p0i8(i64 35, i8* nonnull %49) #3, !dbg !279
  br label %147, !dbg !280

; <label>:51:                                     ; preds = %42
  call void @llvm.dbg.value(metadata i8* %46, metadata !178, metadata !DIExpression()) #3, !dbg !281
  %52 = getelementptr inbounds i8, i8* %15, i64 34, !dbg !282
  %53 = icmp ugt i8* %52, %19, !dbg !283
  br i1 %53, label %54, label %57, !dbg !284

; <label>:54:                                     ; preds = %51
  %55 = getelementptr inbounds [34 x i8], [34 x i8]* %3, i64 0, i64 0, !dbg !209
  call void @llvm.lifetime.start.p0i8(i64 34, i8* nonnull %55) #3, !dbg !209
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* nonnull %55, i8* getelementptr inbounds ([34 x i8], [34 x i8]* @vtl_csum.____fmt.1, i64 0, i64 0), i64 34, i32 1, i1 false) #3, !dbg !209
  %56 = call i32 (i8*, i32, ...) inttoptr (i64 7 to i32 (i8*, i32, ...)*)(i8* nonnull %55, i32 34) #3, !dbg !209
  call void @llvm.lifetime.end.p0i8(i64 34, i8* nonnull %55) #3, !dbg !285
  br label %147, !dbg !286

; <label>:57:                                     ; preds = %51
  %58 = getelementptr inbounds i8, i8* %15, i64 23, !dbg !287
  %59 = load i8, i8* %58, align 1, !dbg !287, !tbaa !249
  %60 = icmp eq i8 %59, -3, !dbg !289
  br i1 %60, label %61, label %147, !dbg !290

; <label>:61:                                     ; preds = %57
  call void @llvm.dbg.value(metadata i8* %52, metadata !186, metadata !DIExpression()) #3, !dbg !291
  %62 = getelementptr inbounds i8, i8* %15, i64 58, !dbg !292
  %63 = icmp ugt i8* %62, %19, !dbg !293
  br i1 %63, label %64, label %67, !dbg !294

; <label>:64:                                     ; preds = %61
  %65 = getelementptr inbounds [35 x i8], [35 x i8]* %4, i64 0, i64 0, !dbg !210
  call void @llvm.lifetime.start.p0i8(i64 35, i8* nonnull %65) #3, !dbg !210
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* nonnull %65, i8* getelementptr inbounds ([35 x i8], [35 x i8]* @vtl_csum.____fmt.2, i64 0, i64 0), i64 35, i32 1, i1 false) #3, !dbg !210
  %66 = call i32 (i8*, i32, ...) inttoptr (i64 7 to i32 (i8*, i32, ...)*)(i8* nonnull %65, i32 35) #3, !dbg !210
  call void @llvm.lifetime.end.p0i8(i64 35, i8* nonnull %65) #3, !dbg !295
  br label %147, !dbg !296

; <label>:67:                                     ; preds = %61
  call void @llvm.dbg.value(metadata i8* %62, metadata !194, metadata !DIExpression()) #3, !dbg !297
  %68 = getelementptr inbounds i8, i8* %15, i64 59, !dbg !298
  %69 = icmp ugt i8* %68, %19, !dbg !299
  br i1 %69, label %70, label %73, !dbg !300

; <label>:70:                                     ; preds = %67
  %71 = getelementptr inbounds [31 x i8], [31 x i8]* %5, i64 0, i64 0, !dbg !211
  call void @llvm.lifetime.start.p0i8(i64 31, i8* nonnull %71) #3, !dbg !211
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* nonnull %71, i8* getelementptr inbounds ([31 x i8], [31 x i8]* @vtl_csum.____fmt.3, i64 0, i64 0), i64 31, i32 1, i1 false) #3, !dbg !211
  %72 = call i32 (i8*, i32, ...) inttoptr (i64 7 to i32 (i8*, i32, ...)*)(i8* nonnull %71, i32 31) #3, !dbg !211
  call void @llvm.lifetime.end.p0i8(i64 31, i8* nonnull %71) #3, !dbg !301
  br label %147, !dbg !302

; <label>:73:                                     ; preds = %67
  call void @llvm.dbg.value(metadata i32 0, metadata !175, metadata !DIExpression()) #3, !dbg !273
  call void @llvm.dbg.value(metadata i8 0, metadata !176, metadata !DIExpression()) #3, !dbg !274
  %74 = getelementptr inbounds i8, i8* %15, i64 42, !dbg !303
  %75 = bitcast i8* %74 to i64*, !dbg !303
  %76 = load i64, i64* %75, align 8, !dbg !303, !tbaa !304
  %77 = icmp eq i64 %76, 0, !dbg !305
  br i1 %77, label %95, label %78, !dbg !306

; <label>:78:                                     ; preds = %73
  br label %79, !dbg !307

; <label>:79:                                     ; preds = %78, %86
  %80 = phi i64 [ %89, %86 ], [ 0, %78 ]
  %81 = phi i8 [ %88, %86 ], [ 0, %78 ]
  call void @llvm.dbg.value(metadata i8 %81, metadata !176, metadata !DIExpression()) #3, !dbg !274
  call void @llvm.dbg.value(metadata i64 %80, metadata !175, metadata !DIExpression()) #3, !dbg !273
  %82 = getelementptr inbounds i8, i8* %62, i64 %80, !dbg !307
  %83 = getelementptr inbounds i8, i8* %82, i64 -1, !dbg !308
  call void @llvm.dbg.value(metadata i8* %83, metadata !202, metadata !DIExpression()) #3, !dbg !309
  %84 = getelementptr inbounds i8, i8* %83, i64 1, !dbg !310
  %85 = icmp ugt i8* %84, %19, !dbg !312
  br i1 %85, label %147, label %86, !dbg !313

; <label>:86:                                     ; preds = %79
  %87 = load i8, i8* %83, align 1, !dbg !314, !tbaa !315
  %88 = xor i8 %87, %81, !dbg !316
  %89 = add nuw nsw i64 %80, 1, !dbg !317
  call void @llvm.dbg.value(metadata i8 %88, metadata !176, metadata !DIExpression()) #3, !dbg !274
  %90 = icmp ule i64 %76, %89, !dbg !305
  %91 = icmp ugt i64 %80, 1023, !dbg !318
  %92 = or i1 %91, %90, !dbg !306
  br i1 %92, label %93, label %79, !dbg !306, !llvm.loop !320

; <label>:93:                                     ; preds = %86
  %94 = zext i8 %88 to i16, !dbg !323
  br label %95, !dbg !323

; <label>:95:                                     ; preds = %93, %73
  %96 = phi i16 [ 0, %73 ], [ %94, %93 ]
  call void @llvm.dbg.value(metadata i16 %96, metadata !159, metadata !DIExpression()), !dbg !270
  %97 = icmp eq i16 %45, %96, !dbg !324
  br i1 %97, label %98, label %114, !dbg !325

; <label>:98:                                     ; preds = %95
  %99 = load i16, i16* %40, align 2, !dbg !326, !tbaa !327
  %100 = getelementptr inbounds [6 x i8], [6 x i8]* %25, i64 3, !dbg !328
  %101 = bitcast [6 x i8]* %100 to i16*, !dbg !328
  %102 = load i16, i16* %101, align 2, !dbg !328, !tbaa !329
  %103 = icmp eq i16 %99, %102, !dbg !330
  br i1 %103, label %104, label %114, !dbg !331

; <label>:104:                                    ; preds = %98
  %105 = call i8* inttoptr (i64 1 to i8* (i8*, i8*)*)(i8* bitcast (%struct.bpf_map_def* @xsks_map to i8*), i8* nonnull %9) #3, !dbg !332
  %106 = icmp eq i8* %105, null, !dbg !332
  br i1 %106, label %147, label %107, !dbg !335

; <label>:107:                                    ; preds = %104
  %108 = load i16, i16* %40, align 2, !dbg !336, !tbaa !327
  %109 = add i16 %108, 1, !dbg !336
  %110 = and i16 %109, 15, !dbg !338
  store i16 %110, i16* %40, align 2, !dbg !338, !tbaa !327
  %111 = call i32 inttoptr (i64 2 to i32 (i8*, i8*, i8*, i64)*)(i8* bitcast (%struct.bpf_map_def* @NUM_ACK_MAP to i8*), i8* nonnull %8, i8* nonnull %39, i64 0) #3, !dbg !339
  %112 = load i32, i32* %7, align 4, !dbg !340, !tbaa !215
  call void @llvm.dbg.value(metadata i32 %112, metadata !151, metadata !DIExpression()), !dbg !222
  %113 = call i32 inttoptr (i64 54 to i32 (i8*, i32, i32)*)(i8* bitcast (%struct.bpf_map_def* @xsks_map to i8*), i32 %112, i32 0) #3, !dbg !341
  br label %147, !dbg !342

; <label>:114:                                    ; preds = %98, %95
  call void @llvm.dbg.value(metadata i8* %15, metadata !343, metadata !DIExpression()), !dbg !354
  %115 = inttoptr i64 %14 to i16*, !dbg !356
  call void @llvm.dbg.value(metadata i16* %115, metadata !348, metadata !DIExpression()), !dbg !357
  %116 = load i16, i16* %115, align 2, !dbg !358, !tbaa !327
  call void @llvm.dbg.value(metadata i16 %116, metadata !350, metadata !DIExpression(DW_OP_LLVM_fragment, 0, 16)), !dbg !359
  %117 = getelementptr inbounds i8, i8* %15, i64 2, !dbg !360
  %118 = bitcast i8* %117 to i16*, !dbg !360
  %119 = load i16, i16* %118, align 2, !dbg !360, !tbaa !327
  call void @llvm.dbg.value(metadata i16 %119, metadata !350, metadata !DIExpression(DW_OP_LLVM_fragment, 16, 16)), !dbg !359
  %120 = getelementptr inbounds i8, i8* %15, i64 4, !dbg !361
  %121 = bitcast i8* %120 to i16*, !dbg !361
  %122 = load i16, i16* %121, align 2, !dbg !361, !tbaa !327
  call void @llvm.dbg.value(metadata i16 %122, metadata !350, metadata !DIExpression(DW_OP_LLVM_fragment, 32, 16)), !dbg !359
  %123 = getelementptr inbounds i8, i8* %15, i64 6, !dbg !362
  %124 = bitcast i8* %123 to i16*, !dbg !362
  %125 = load i16, i16* %124, align 2, !dbg !362, !tbaa !327
  store i16 %125, i16* %115, align 2, !dbg !363, !tbaa !327
  %126 = getelementptr inbounds i8, i8* %15, i64 8, !dbg !364
  %127 = bitcast i8* %126 to i16*, !dbg !364
  %128 = load i16, i16* %127, align 2, !dbg !364, !tbaa !327
  store i16 %128, i16* %118, align 2, !dbg !365, !tbaa !327
  %129 = getelementptr inbounds i8, i8* %15, i64 10, !dbg !366
  %130 = bitcast i8* %129 to i16*, !dbg !366
  %131 = load i16, i16* %130, align 2, !dbg !366, !tbaa !327
  store i16 %131, i16* %121, align 2, !dbg !367, !tbaa !327
  store i16 %116, i16* %124, align 2, !dbg !368, !tbaa !327
  store i16 %119, i16* %127, align 2, !dbg !369, !tbaa !327
  store i16 %122, i16* %130, align 2, !dbg !370, !tbaa !327
  %132 = getelementptr inbounds %struct.ethhdr, %struct.ethhdr* %20, i64 1, i32 2, !dbg !371
  %133 = bitcast i16* %132 to i32*, !dbg !371
  %134 = load i32, i32* %133, align 4, !dbg !371, !tbaa !372
  call void @llvm.dbg.value(metadata i32 %134, metadata !160, metadata !DIExpression()), !dbg !373
  %135 = getelementptr inbounds %struct.ethhdr, %struct.ethhdr* %20, i64 2, i32 0, i64 2, !dbg !374
  %136 = bitcast i8* %135 to i32*, !dbg !374
  %137 = load i32, i32* %136, align 4, !dbg !374, !tbaa !375
  store i32 %137, i32* %133, align 4, !dbg !376, !tbaa !372
  store i32 %134, i32* %136, align 4, !dbg !377, !tbaa !375
  %138 = getelementptr inbounds %struct.ethhdr, %struct.ethhdr* %20, i64 2, i32 1, i64 4, !dbg !378
  %139 = bitcast i8* %138 to i32*, !dbg !378
  store i32 3, i32* %139, align 4, !dbg !379, !tbaa !380
  %140 = bitcast %struct.xdp_md* %0 to i8*, !dbg !381
  %141 = getelementptr inbounds [6 x i8], [6 x i8]* %25, i64 1, i64 2, !dbg !382
  %142 = bitcast i8* %141 to i64*, !dbg !382
  %143 = load i64, i64* %142, align 8, !dbg !382, !tbaa !304
  %144 = trunc i64 %143 to i32, !dbg !383
  %145 = sub i32 0, %144, !dbg !383
  %146 = call i32 inttoptr (i64 68 to i32 (i8*, i32)*)(i8* %140, i32 %145) #3, !dbg !384
  br label %147

; <label>:147:                                    ; preds = %79, %70, %64, %57, %54, %48, %107, %114, %104, %24, %29, %38, %33, %1
  %148 = phi i32 [ 1, %1 ], [ 1, %24 ], [ 2, %29 ], [ 1, %33 ], [ 2, %38 ], [ %113, %107 ], [ 3, %114 ], [ 2, %104 ], [ 2, %48 ], [ 2, %54 ], [ 2, %57 ], [ 2, %64 ], [ 2, %70 ], [ 2, %79 ]
  call void @llvm.lifetime.end.p0i8(i64 4, i8* nonnull %9) #3, !dbg !385
  call void @llvm.lifetime.end.p0i8(i64 4, i8* nonnull %8) #3, !dbg !385
  ret i32 %148, !dbg !385
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.start.p0i8(i64, i8* nocapture) #2

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.end.p0i8(i64, i8* nocapture) #2

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i32, i1) #2

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.value(metadata, metadata, metadata) #1

attributes #0 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { argmemonly nounwind }
attributes #3 = { nounwind }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!132, !133, !134}
!llvm.ident = !{!135}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "xsks_map", scope: !2, file: !3, line: 16, type: !87, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 6.0.0-1ubuntu2 (tags/RELEASE_600/final)", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug, enums: !4, retainedTypes: !22, globals: !84)
!3 = !DIFile(filename: "ingress_test_graft.c", directory: "/home/vtl_server/vtlWorkspace/vtl_server/XTFsPool")
!4 = !{!5, !13}
!5 = !DICompositeType(tag: DW_TAG_enumeration_type, name: "xdp_action", file: !6, line: 2848, size: 32, elements: !7)
!6 = !DIFile(filename: "./include/linux/bpf.h", directory: "/home/vtl_server/vtlWorkspace/vtl_server/XTFsPool")
!7 = !{!8, !9, !10, !11, !12}
!8 = !DIEnumerator(name: "XDP_ABORTED", value: 0)
!9 = !DIEnumerator(name: "XDP_DROP", value: 1)
!10 = !DIEnumerator(name: "XDP_PASS", value: 2)
!11 = !DIEnumerator(name: "XDP_TX", value: 3)
!12 = !DIEnumerator(name: "XDP_REDIRECT", value: 4)
!13 = !DICompositeType(tag: DW_TAG_enumeration_type, file: !14, line: 66, size: 32, elements: !15)
!14 = !DIFile(filename: "./../include/vtl.h", directory: "/home/vtl_server/vtlWorkspace/vtl_server/XTFsPool")
!15 = !{!16, !17, !18, !19, !20, !21}
!16 = !DIEnumerator(name: "NEGO", value: 0)
!17 = !DIEnumerator(name: "DATA", value: 1)
!18 = !DIEnumerator(name: "ACK", value: 2)
!19 = !DIEnumerator(name: "NACK", value: 3)
!20 = !DIEnumerator(name: "NEGO_ACK", value: 4)
!21 = !DIEnumerator(name: "NEGO_NACK", value: 5)
!22 = !{!23, !24, !25, !41, !65, !82, !83, !54}
!23 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: 64)
!24 = !DIBasicType(name: "long int", size: 64, encoding: DW_ATE_signed)
!25 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !26, size: 64)
!26 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "ethhdr", file: !27, line: 163, size: 112, elements: !28)
!27 = !DIFile(filename: "./include/linux/if_ether.h", directory: "/home/vtl_server/vtlWorkspace/vtl_server/XTFsPool")
!28 = !{!29, !34, !35}
!29 = !DIDerivedType(tag: DW_TAG_member, name: "h_dest", scope: !26, file: !27, line: 164, baseType: !30, size: 48)
!30 = !DICompositeType(tag: DW_TAG_array_type, baseType: !31, size: 48, elements: !32)
!31 = !DIBasicType(name: "unsigned char", size: 8, encoding: DW_ATE_unsigned_char)
!32 = !{!33}
!33 = !DISubrange(count: 6)
!34 = !DIDerivedType(tag: DW_TAG_member, name: "h_source", scope: !26, file: !27, line: 165, baseType: !30, size: 48, offset: 48)
!35 = !DIDerivedType(tag: DW_TAG_member, name: "h_proto", scope: !26, file: !27, line: 166, baseType: !36, size: 16, offset: 96)
!36 = !DIDerivedType(tag: DW_TAG_typedef, name: "__be16", file: !37, line: 25, baseType: !38)
!37 = !DIFile(filename: "/usr/include/linux/types.h", directory: "/home/vtl_server/vtlWorkspace/vtl_server/XTFsPool")
!38 = !DIDerivedType(tag: DW_TAG_typedef, name: "__u16", file: !39, line: 24, baseType: !40)
!39 = !DIFile(filename: "/usr/include/asm-generic/int-ll64.h", directory: "/home/vtl_server/vtlWorkspace/vtl_server/XTFsPool")
!40 = !DIBasicType(name: "unsigned short", size: 16, encoding: DW_ATE_unsigned)
!41 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !42, size: 64)
!42 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "iphdr", file: !43, line: 44, size: 160, elements: !44)
!43 = !DIFile(filename: "/usr/include/netinet/ip.h", directory: "/home/vtl_server/vtlWorkspace/vtl_server/XTFsPool")
!44 = !{!45, !47, !48, !53, !56, !57, !58, !59, !60, !61, !64}
!45 = !DIDerivedType(tag: DW_TAG_member, name: "ihl", scope: !42, file: !43, line: 47, baseType: !46, size: 4, flags: DIFlagBitField, extraData: i64 0)
!46 = !DIBasicType(name: "unsigned int", size: 32, encoding: DW_ATE_unsigned)
!47 = !DIDerivedType(tag: DW_TAG_member, name: "version", scope: !42, file: !43, line: 48, baseType: !46, size: 4, offset: 4, flags: DIFlagBitField, extraData: i64 0)
!48 = !DIDerivedType(tag: DW_TAG_member, name: "tos", scope: !42, file: !43, line: 55, baseType: !49, size: 8, offset: 8)
!49 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint8_t", file: !50, line: 24, baseType: !51)
!50 = !DIFile(filename: "/usr/include/bits/stdint-uintn.h", directory: "/home/vtl_server/vtlWorkspace/vtl_server/XTFsPool")
!51 = !DIDerivedType(tag: DW_TAG_typedef, name: "__uint8_t", file: !52, line: 37, baseType: !31)
!52 = !DIFile(filename: "/usr/include/bits/types.h", directory: "/home/vtl_server/vtlWorkspace/vtl_server/XTFsPool")
!53 = !DIDerivedType(tag: DW_TAG_member, name: "tot_len", scope: !42, file: !43, line: 56, baseType: !54, size: 16, offset: 16)
!54 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint16_t", file: !50, line: 25, baseType: !55)
!55 = !DIDerivedType(tag: DW_TAG_typedef, name: "__uint16_t", file: !52, line: 39, baseType: !40)
!56 = !DIDerivedType(tag: DW_TAG_member, name: "id", scope: !42, file: !43, line: 57, baseType: !54, size: 16, offset: 32)
!57 = !DIDerivedType(tag: DW_TAG_member, name: "frag_off", scope: !42, file: !43, line: 58, baseType: !54, size: 16, offset: 48)
!58 = !DIDerivedType(tag: DW_TAG_member, name: "ttl", scope: !42, file: !43, line: 59, baseType: !49, size: 8, offset: 64)
!59 = !DIDerivedType(tag: DW_TAG_member, name: "protocol", scope: !42, file: !43, line: 60, baseType: !49, size: 8, offset: 72)
!60 = !DIDerivedType(tag: DW_TAG_member, name: "check", scope: !42, file: !43, line: 61, baseType: !54, size: 16, offset: 80)
!61 = !DIDerivedType(tag: DW_TAG_member, name: "saddr", scope: !42, file: !43, line: 62, baseType: !62, size: 32, offset: 96)
!62 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint32_t", file: !50, line: 26, baseType: !63)
!63 = !DIDerivedType(tag: DW_TAG_typedef, name: "__uint32_t", file: !52, line: 41, baseType: !46)
!64 = !DIDerivedType(tag: DW_TAG_member, name: "daddr", scope: !42, file: !43, line: 63, baseType: !62, size: 32, offset: 128)
!65 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !66, size: 64)
!66 = !DIDerivedType(tag: DW_TAG_typedef, name: "vtl_hdr_t", file: !14, line: 84, baseType: !67)
!67 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !14, line: 78, size: 192, elements: !68)
!68 = !{!69, !74, !76, !80, !81}
!69 = !DIDerivedType(tag: DW_TAG_member, name: "gid", scope: !67, file: !14, line: 79, baseType: !70, size: 8)
!70 = !DIDerivedType(tag: DW_TAG_typedef, name: "int8_t", file: !71, line: 24, baseType: !72)
!71 = !DIFile(filename: "/usr/include/bits/stdint-intn.h", directory: "/home/vtl_server/vtlWorkspace/vtl_server/XTFsPool")
!72 = !DIDerivedType(tag: DW_TAG_typedef, name: "__int8_t", file: !52, line: 36, baseType: !73)
!73 = !DIBasicType(name: "signed char", size: 8, encoding: DW_ATE_signed_char)
!74 = !DIDerivedType(tag: DW_TAG_member, name: "pkt_type", scope: !67, file: !14, line: 80, baseType: !75, size: 32, offset: 32)
!75 = !DIDerivedType(tag: DW_TAG_typedef, name: "vtl_pkt_type", file: !14, line: 73, baseType: !13)
!76 = !DIDerivedType(tag: DW_TAG_member, name: "payload_len", scope: !67, file: !14, line: 81, baseType: !77, size: 64, offset: 64)
!77 = !DIDerivedType(tag: DW_TAG_typedef, name: "size_t", file: !78, line: 62, baseType: !79)
!78 = !DIFile(filename: "/usr/lib/llvm-6.0/lib/clang/6.0.0/include/stddef.h", directory: "/home/vtl_server/vtlWorkspace/vtl_server/XTFsPool")
!79 = !DIBasicType(name: "long unsigned int", size: 64, encoding: DW_ATE_unsigned)
!80 = !DIDerivedType(tag: DW_TAG_member, name: "checksum", scope: !67, file: !14, line: 82, baseType: !54, size: 16, offset: 128)
!81 = !DIDerivedType(tag: DW_TAG_member, name: "seq_num", scope: !67, file: !14, line: 83, baseType: !54, size: 16, offset: 144)
!82 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !54, size: 64)
!83 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !49, size: 64)
!84 = !{!0, !85, !97, !103, !108, !116, !122, !127}
!85 = !DIGlobalVariableExpression(var: !86, expr: !DIExpression())
!86 = distinct !DIGlobalVariable(name: "NUM_ACK_MAP", scope: !2, file: !3, line: 23, type: !87, isLocal: false, isDefinition: true)
!87 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "bpf_map_def", file: !88, line: 214, size: 224, elements: !89)
!88 = !DIFile(filename: "./include/bpf/bpf_helpers.h", directory: "/home/vtl_server/vtlWorkspace/vtl_server/XTFsPool")
!89 = !{!90, !91, !92, !93, !94, !95, !96}
!90 = !DIDerivedType(tag: DW_TAG_member, name: "type", scope: !87, file: !88, line: 215, baseType: !46, size: 32)
!91 = !DIDerivedType(tag: DW_TAG_member, name: "key_size", scope: !87, file: !88, line: 216, baseType: !46, size: 32, offset: 32)
!92 = !DIDerivedType(tag: DW_TAG_member, name: "value_size", scope: !87, file: !88, line: 217, baseType: !46, size: 32, offset: 64)
!93 = !DIDerivedType(tag: DW_TAG_member, name: "max_entries", scope: !87, file: !88, line: 218, baseType: !46, size: 32, offset: 96)
!94 = !DIDerivedType(tag: DW_TAG_member, name: "map_flags", scope: !87, file: !88, line: 219, baseType: !46, size: 32, offset: 128)
!95 = !DIDerivedType(tag: DW_TAG_member, name: "inner_map_idx", scope: !87, file: !88, line: 220, baseType: !46, size: 32, offset: 160)
!96 = !DIDerivedType(tag: DW_TAG_member, name: "numa_node", scope: !87, file: !88, line: 221, baseType: !46, size: 32, offset: 192)
!97 = !DIGlobalVariableExpression(var: !98, expr: !DIExpression())
!98 = distinct !DIGlobalVariable(name: "_license", scope: !2, file: !3, line: 110, type: !99, isLocal: false, isDefinition: true)
!99 = !DICompositeType(tag: DW_TAG_array_type, baseType: !100, size: 32, elements: !101)
!100 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!101 = !{!102}
!102 = !DISubrange(count: 4)
!103 = !DIGlobalVariableExpression(var: !104, expr: !DIExpression())
!104 = distinct !DIGlobalVariable(name: "bpf_map_lookup_elem", scope: !2, file: !88, line: 20, type: !105, isLocal: true, isDefinition: true)
!105 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !106, size: 64)
!106 = !DISubroutineType(types: !107)
!107 = !{!23, !23, !23}
!108 = !DIGlobalVariableExpression(var: !109, expr: !DIExpression())
!109 = distinct !DIGlobalVariable(name: "bpf_trace_printk", scope: !2, file: !88, line: 40, type: !110, isLocal: true, isDefinition: true)
!110 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !111, size: 64)
!111 = !DISubroutineType(types: !112)
!112 = !{!113, !114, !113, null}
!113 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!114 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !115, size: 64)
!115 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !100)
!116 = !DIGlobalVariableExpression(var: !117, expr: !DIExpression())
!117 = distinct !DIGlobalVariable(name: "bpf_map_update_elem", scope: !2, file: !88, line: 22, type: !118, isLocal: true, isDefinition: true)
!118 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !119, size: 64)
!119 = !DISubroutineType(types: !120)
!120 = !{!113, !23, !23, !23, !121}
!121 = !DIBasicType(name: "long long unsigned int", size: 64, encoding: DW_ATE_unsigned)
!122 = !DIGlobalVariableExpression(var: !123, expr: !DIExpression())
!123 = distinct !DIGlobalVariable(name: "bpf_redirect_map", scope: !2, file: !88, line: 61, type: !124, isLocal: true, isDefinition: true)
!124 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !125, size: 64)
!125 = !DISubroutineType(types: !126)
!126 = !{!113, !23, !113, !113}
!127 = !DIGlobalVariableExpression(var: !128, expr: !DIExpression())
!128 = distinct !DIGlobalVariable(name: "bpf_xdp_adjust_tail", scope: !2, file: !88, line: 132, type: !129, isLocal: true, isDefinition: true)
!129 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !130, size: 64)
!130 = !DISubroutineType(types: !131)
!131 = !{!113, !23, !113}
!132 = !{i32 2, !"Dwarf Version", i32 4}
!133 = !{i32 2, !"Debug Info Version", i32 3}
!134 = !{i32 1, !"wchar_size", i32 4}
!135 = !{!"clang version 6.0.0-1ubuntu2 (tags/RELEASE_600/final)"}
!136 = distinct !DISubprogram(name: "xdp_sock_prog", scope: !3, file: !3, line: 47, type: !137, isLocal: false, isDefinition: true, scopeLine: 47, flags: DIFlagPrototyped, isOptimized: true, unit: !2, variables: !148)
!137 = !DISubroutineType(types: !138)
!138 = !{!113, !139}
!139 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !140, size: 64)
!140 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "xdp_md", file: !6, line: 2859, size: 160, elements: !141)
!141 = !{!142, !144, !145, !146, !147}
!142 = !DIDerivedType(tag: DW_TAG_member, name: "data", scope: !140, file: !6, line: 2860, baseType: !143, size: 32)
!143 = !DIDerivedType(tag: DW_TAG_typedef, name: "__u32", file: !39, line: 27, baseType: !46)
!144 = !DIDerivedType(tag: DW_TAG_member, name: "data_end", scope: !140, file: !6, line: 2861, baseType: !143, size: 32, offset: 32)
!145 = !DIDerivedType(tag: DW_TAG_member, name: "data_meta", scope: !140, file: !6, line: 2862, baseType: !143, size: 32, offset: 64)
!146 = !DIDerivedType(tag: DW_TAG_member, name: "ingress_ifindex", scope: !140, file: !6, line: 2864, baseType: !143, size: 32, offset: 96)
!147 = !DIDerivedType(tag: DW_TAG_member, name: "rx_queue_index", scope: !140, file: !6, line: 2865, baseType: !143, size: 32, offset: 128)
!148 = !{!149, !150, !151, !152, !153, !154, !155, !156, !157, !158, !159, !160}
!149 = !DILocalVariable(name: "ctx", arg: 1, scope: !136, file: !3, line: 47, type: !139)
!150 = !DILocalVariable(name: "index", scope: !136, file: !3, line: 49, type: !113)
!151 = !DILocalVariable(name: "xsk_index", scope: !136, file: !3, line: 49, type: !113)
!152 = !DILocalVariable(name: "ack_num", scope: !136, file: !3, line: 50, type: !82)
!153 = !DILocalVariable(name: "data", scope: !136, file: !3, line: 52, type: !23)
!154 = !DILocalVariable(name: "data_end", scope: !136, file: !3, line: 53, type: !23)
!155 = !DILocalVariable(name: "eth", scope: !136, file: !3, line: 55, type: !25)
!156 = !DILocalVariable(name: "iph", scope: !136, file: !3, line: 59, type: !41)
!157 = !DILocalVariable(name: "vtlh", scope: !136, file: !3, line: 67, type: !65)
!158 = !DILocalVariable(name: "recv_csum", scope: !136, file: !3, line: 75, type: !54)
!159 = !DILocalVariable(name: "compute_csum", scope: !136, file: !3, line: 77, type: !54)
!160 = !DILocalVariable(name: "temp_ip", scope: !161, file: !3, line: 95, type: !163)
!161 = distinct !DILexicalBlock(scope: !162, file: !3, line: 93, column: 14)
!162 = distinct !DILexicalBlock(scope: !136, file: !3, line: 81, column: 12)
!163 = !DIDerivedType(tag: DW_TAG_typedef, name: "__be32", file: !37, line: 27, baseType: !143)
!164 = !DILocalVariable(name: "____fmt", scope: !165, file: !14, line: 228, type: !191)
!165 = distinct !DILexicalBlock(scope: !166, file: !14, line: 228, column: 7)
!166 = distinct !DILexicalBlock(scope: !167, file: !14, line: 227, column: 27)
!167 = distinct !DILexicalBlock(scope: !168, file: !14, line: 227, column: 7)
!168 = distinct !DISubprogram(name: "vtl_csum", scope: !14, file: !14, line: 221, type: !169, isLocal: true, isDefinition: true, scopeLine: 221, flags: DIFlagPrototyped, isOptimized: true, unit: !2, variables: !171)
!169 = !DISubroutineType(types: !170)
!170 = !{!113, !23, !23, !82}
!171 = !{!172, !173, !174, !175, !176, !177, !164, !178, !179, !186, !187, !194, !195, !202}
!172 = !DILocalVariable(name: "data", arg: 1, scope: !168, file: !14, line: 221, type: !23)
!173 = !DILocalVariable(name: "data_end", arg: 2, scope: !168, file: !14, line: 221, type: !23)
!174 = !DILocalVariable(name: "csum", arg: 3, scope: !168, file: !14, line: 221, type: !82)
!175 = !DILocalVariable(name: "y", scope: !168, file: !14, line: 223, type: !113)
!176 = !DILocalVariable(name: "sum", scope: !168, file: !14, line: 224, type: !49)
!177 = !DILocalVariable(name: "eth", scope: !168, file: !14, line: 226, type: !25)
!178 = !DILocalVariable(name: "iph", scope: !168, file: !14, line: 232, type: !41)
!179 = !DILocalVariable(name: "____fmt", scope: !180, file: !14, line: 234, type: !183)
!180 = distinct !DILexicalBlock(scope: !181, file: !14, line: 234, column: 7)
!181 = distinct !DILexicalBlock(scope: !182, file: !14, line: 233, column: 27)
!182 = distinct !DILexicalBlock(scope: !168, file: !14, line: 233, column: 7)
!183 = !DICompositeType(tag: DW_TAG_array_type, baseType: !100, size: 272, elements: !184)
!184 = !{!185}
!185 = !DISubrange(count: 34)
!186 = !DILocalVariable(name: "vtlh", scope: !168, file: !14, line: 242, type: !65)
!187 = !DILocalVariable(name: "____fmt", scope: !188, file: !14, line: 244, type: !191)
!188 = distinct !DILexicalBlock(scope: !189, file: !14, line: 244, column: 7)
!189 = distinct !DILexicalBlock(scope: !190, file: !14, line: 243, column: 28)
!190 = distinct !DILexicalBlock(scope: !168, file: !14, line: 243, column: 7)
!191 = !DICompositeType(tag: DW_TAG_array_type, baseType: !100, size: 280, elements: !192)
!192 = !{!193}
!193 = !DISubrange(count: 35)
!194 = !DILocalVariable(name: "d", scope: !168, file: !14, line: 248, type: !83)
!195 = !DILocalVariable(name: "____fmt", scope: !196, file: !14, line: 250, type: !199)
!196 = distinct !DILexicalBlock(scope: !197, file: !14, line: 250, column: 7)
!197 = distinct !DILexicalBlock(scope: !198, file: !14, line: 249, column: 25)
!198 = distinct !DILexicalBlock(scope: !168, file: !14, line: 249, column: 7)
!199 = !DICompositeType(tag: DW_TAG_array_type, baseType: !100, size: 248, elements: !200)
!200 = !{!201}
!201 = !DISubrange(count: 31)
!202 = !DILocalVariable(name: "block", scope: !203, file: !14, line: 259, type: !83)
!203 = distinct !DILexicalBlock(scope: !204, file: !14, line: 254, column: 43)
!204 = distinct !DILexicalBlock(scope: !205, file: !14, line: 254, column: 4)
!205 = distinct !DILexicalBlock(scope: !168, file: !14, line: 254, column: 4)
!206 = !DILocation(line: 228, column: 7, scope: !165, inlinedAt: !207)
!207 = distinct !DILocation(line: 78, column: 12, scope: !208)
!208 = distinct !DILexicalBlock(scope: !136, file: !3, line: 78, column: 12)
!209 = !DILocation(line: 234, column: 7, scope: !180, inlinedAt: !207)
!210 = !DILocation(line: 244, column: 7, scope: !188, inlinedAt: !207)
!211 = !DILocation(line: 250, column: 7, scope: !196, inlinedAt: !207)
!212 = !DILocation(line: 47, column: 34, scope: !136)
!213 = !DILocation(line: 49, column: 9, scope: !136)
!214 = !DILocation(line: 49, column: 13, scope: !136)
!215 = !{!216, !216, i64 0}
!216 = !{!"int", !217, i64 0}
!217 = !{!"omnipotent char", !218, i64 0}
!218 = !{!"Simple C/C++ TBAA"}
!219 = !DILocation(line: 49, column: 41, scope: !136)
!220 = !{!221, !216, i64 16}
!221 = !{!"xdp_md", !216, i64 0, !216, i64 4, !216, i64 8, !216, i64 12, !216, i64 16}
!222 = !DILocation(line: 49, column: 24, scope: !136)
!223 = !DILocation(line: 50, column: 19, scope: !136)
!224 = !DILocation(line: 52, column: 41, scope: !136)
!225 = !{!221, !216, i64 0}
!226 = !DILocation(line: 52, column: 30, scope: !136)
!227 = !DILocation(line: 52, column: 22, scope: !136)
!228 = !DILocation(line: 52, column: 15, scope: !136)
!229 = !DILocation(line: 53, column: 45, scope: !136)
!230 = !{!221, !216, i64 4}
!231 = !DILocation(line: 53, column: 34, scope: !136)
!232 = !DILocation(line: 53, column: 26, scope: !136)
!233 = !DILocation(line: 53, column: 15, scope: !136)
!234 = !DILocation(line: 55, column: 30, scope: !136)
!235 = !DILocation(line: 55, column: 24, scope: !136)
!236 = !DILocation(line: 56, column: 17, scope: !237)
!237 = distinct !DILexicalBlock(scope: !136, file: !3, line: 56, column: 13)
!238 = !DILocation(line: 56, column: 23, scope: !237)
!239 = !DILocation(line: 56, column: 21, scope: !237)
!240 = !DILocation(line: 56, column: 13, scope: !136)
!241 = !DILocation(line: 59, column: 23, scope: !136)
!242 = !DILocation(line: 60, column: 17, scope: !243)
!243 = distinct !DILexicalBlock(scope: !136, file: !3, line: 60, column: 13)
!244 = !DILocation(line: 60, column: 23, scope: !243)
!245 = !DILocation(line: 60, column: 21, scope: !243)
!246 = !DILocation(line: 60, column: 13, scope: !136)
!247 = !DILocation(line: 63, column: 17, scope: !248)
!248 = distinct !DILexicalBlock(scope: !136, file: !3, line: 63, column: 12)
!249 = !{!250, !217, i64 9}
!250 = !{!"iphdr", !216, i64 0, !216, i64 0, !217, i64 1, !251, i64 2, !251, i64 4, !251, i64 6, !217, i64 8, !217, i64 9, !251, i64 10, !216, i64 12, !216, i64 16}
!251 = !{!"short", !217, i64 0}
!252 = !DILocation(line: 63, column: 26, scope: !248)
!253 = !DILocation(line: 63, column: 12, scope: !136)
!254 = !DILocation(line: 67, column: 20, scope: !136)
!255 = !DILocation(line: 68, column: 17, scope: !256)
!256 = distinct !DILexicalBlock(scope: !136, file: !3, line: 68, column: 12)
!257 = !DILocation(line: 68, column: 23, scope: !256)
!258 = !DILocation(line: 68, column: 21, scope: !256)
!259 = !DILocation(line: 68, column: 12, scope: !136)
!260 = !DILocation(line: 71, column: 32, scope: !136)
!261 = !DILocation(line: 71, column: 19, scope: !136)
!262 = !DILocation(line: 72, column: 13, scope: !263)
!263 = distinct !DILexicalBlock(scope: !136, file: !3, line: 72, column: 12)
!264 = !DILocation(line: 72, column: 12, scope: !136)
!265 = !DILocation(line: 75, column: 36, scope: !136)
!266 = !{!267, !251, i64 16}
!267 = !{!"", !217, i64 0, !217, i64 4, !268, i64 8, !251, i64 16, !251, i64 18}
!268 = !{!"long", !217, i64 0}
!269 = !DILocation(line: 75, column: 18, scope: !136)
!270 = !DILocation(line: 77, column: 18, scope: !136)
!271 = !DILocation(line: 221, column: 43, scope: !168, inlinedAt: !207)
!272 = !DILocation(line: 221, column: 55, scope: !168, inlinedAt: !207)
!273 = !DILocation(line: 223, column: 6, scope: !168, inlinedAt: !207)
!274 = !DILocation(line: 224, column: 10, scope: !168, inlinedAt: !207)
!275 = !DILocation(line: 226, column: 17, scope: !168, inlinedAt: !207)
!276 = !DILocation(line: 227, column: 11, scope: !167, inlinedAt: !207)
!277 = !DILocation(line: 227, column: 15, scope: !167, inlinedAt: !207)
!278 = !DILocation(line: 227, column: 7, scope: !168, inlinedAt: !207)
!279 = !DILocation(line: 228, column: 7, scope: !166, inlinedAt: !207)
!280 = !DILocation(line: 229, column: 7, scope: !166, inlinedAt: !207)
!281 = !DILocation(line: 232, column: 18, scope: !168, inlinedAt: !207)
!282 = !DILocation(line: 233, column: 11, scope: !182, inlinedAt: !207)
!283 = !DILocation(line: 233, column: 15, scope: !182, inlinedAt: !207)
!284 = !DILocation(line: 233, column: 7, scope: !168, inlinedAt: !207)
!285 = !DILocation(line: 234, column: 7, scope: !181, inlinedAt: !207)
!286 = !DILocation(line: 235, column: 7, scope: !181, inlinedAt: !207)
!287 = !DILocation(line: 238, column: 12, scope: !288, inlinedAt: !207)
!288 = distinct !DILexicalBlock(scope: !168, file: !14, line: 238, column: 7)
!289 = !DILocation(line: 238, column: 21, scope: !288, inlinedAt: !207)
!290 = !DILocation(line: 238, column: 7, scope: !168, inlinedAt: !207)
!291 = !DILocation(line: 242, column: 15, scope: !168, inlinedAt: !207)
!292 = !DILocation(line: 243, column: 12, scope: !190, inlinedAt: !207)
!293 = !DILocation(line: 243, column: 16, scope: !190, inlinedAt: !207)
!294 = !DILocation(line: 243, column: 7, scope: !168, inlinedAt: !207)
!295 = !DILocation(line: 244, column: 7, scope: !189, inlinedAt: !207)
!296 = !DILocation(line: 245, column: 7, scope: !189, inlinedAt: !207)
!297 = !DILocation(line: 248, column: 13, scope: !168, inlinedAt: !207)
!298 = !DILocation(line: 249, column: 9, scope: !198, inlinedAt: !207)
!299 = !DILocation(line: 249, column: 13, scope: !198, inlinedAt: !207)
!300 = !DILocation(line: 249, column: 7, scope: !168, inlinedAt: !207)
!301 = !DILocation(line: 250, column: 7, scope: !197, inlinedAt: !207)
!302 = !DILocation(line: 251, column: 7, scope: !197, inlinedAt: !207)
!303 = !DILocation(line: 254, column: 25, scope: !204, inlinedAt: !207)
!304 = !{!267, !268, i64 8}
!305 = !DILocation(line: 254, column: 17, scope: !204, inlinedAt: !207)
!306 = !DILocation(line: 254, column: 4, scope: !205, inlinedAt: !207)
!307 = !DILocation(line: 259, column: 38, scope: !203, inlinedAt: !207)
!308 = !DILocation(line: 259, column: 42, scope: !203, inlinedAt: !207)
!309 = !DILocation(line: 259, column: 16, scope: !203, inlinedAt: !207)
!310 = !DILocation(line: 260, column: 16, scope: !311, inlinedAt: !207)
!311 = distinct !DILexicalBlock(scope: !203, file: !14, line: 260, column: 10)
!312 = !DILocation(line: 260, column: 20, scope: !311, inlinedAt: !207)
!313 = !DILocation(line: 260, column: 10, scope: !203, inlinedAt: !207)
!314 = !DILocation(line: 263, column: 14, scope: !203, inlinedAt: !207)
!315 = !{!217, !217, i64 0}
!316 = !DILocation(line: 263, column: 11, scope: !203, inlinedAt: !207)
!317 = !DILocation(line: 254, column: 39, scope: !204, inlinedAt: !207)
!318 = !DILocation(line: 256, column: 10, scope: !319, inlinedAt: !207)
!319 = distinct !DILexicalBlock(scope: !203, file: !14, line: 256, column: 8)
!320 = distinct !{!320, !321, !322}
!321 = !DILocation(line: 254, column: 4, scope: !205)
!322 = !DILocation(line: 264, column: 4, scope: !205)
!323 = !DILocation(line: 265, column: 12, scope: !168, inlinedAt: !207)
!324 = !DILocation(line: 81, column: 22, scope: !162)
!325 = !DILocation(line: 81, column: 38, scope: !162)
!326 = !DILocation(line: 82, column: 17, scope: !162)
!327 = !{!251, !251, i64 0}
!328 = !DILocation(line: 82, column: 35, scope: !162)
!329 = !{!267, !251, i64 18}
!330 = !DILocation(line: 82, column: 26, scope: !162)
!331 = !DILocation(line: 81, column: 12, scope: !136)
!332 = !DILocation(line: 84, column: 21, scope: !333)
!333 = distinct !DILexicalBlock(scope: !334, file: !3, line: 84, column: 21)
!334 = distinct !DILexicalBlock(scope: !162, file: !3, line: 82, column: 44)
!335 = !DILocation(line: 84, column: 21, scope: !334)
!336 = !DILocation(line: 86, column: 35, scope: !337)
!337 = distinct !DILexicalBlock(scope: !333, file: !3, line: 84, column: 65)
!338 = !DILocation(line: 87, column: 34, scope: !337)
!339 = !DILocation(line: 88, column: 25, scope: !337)
!340 = !DILocation(line: 90, column: 60, scope: !337)
!341 = !DILocation(line: 90, column: 32, scope: !337)
!342 = !DILocation(line: 90, column: 25, scope: !337)
!343 = !DILocalVariable(name: "data", arg: 1, scope: !344, file: !3, line: 30, type: !23)
!344 = distinct !DISubprogram(name: "swap_src_dst_mac", scope: !3, file: !3, line: 30, type: !345, isLocal: true, isDefinition: true, scopeLine: 30, flags: DIFlagPrototyped, isOptimized: true, unit: !2, variables: !347)
!345 = !DISubroutineType(types: !346)
!346 = !{null, !23}
!347 = !{!343, !348, !350}
!348 = !DILocalVariable(name: "p", scope: !344, file: !3, line: 32, type: !349)
!349 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !40, size: 64)
!350 = !DILocalVariable(name: "dst", scope: !344, file: !3, line: 33, type: !351)
!351 = !DICompositeType(tag: DW_TAG_array_type, baseType: !40, size: 48, elements: !352)
!352 = !{!353}
!353 = !DISubrange(count: 3)
!354 = !DILocation(line: 30, column: 52, scope: !344, inlinedAt: !355)
!355 = distinct !DILocation(line: 94, column: 17, scope: !161)
!356 = !DILocation(line: 32, column: 29, scope: !344, inlinedAt: !355)
!357 = !DILocation(line: 32, column: 25, scope: !344, inlinedAt: !355)
!358 = !DILocation(line: 35, column: 18, scope: !344, inlinedAt: !355)
!359 = !DILocation(line: 33, column: 24, scope: !344, inlinedAt: !355)
!360 = !DILocation(line: 36, column: 18, scope: !344, inlinedAt: !355)
!361 = !DILocation(line: 37, column: 18, scope: !344, inlinedAt: !355)
!362 = !DILocation(line: 38, column: 16, scope: !344, inlinedAt: !355)
!363 = !DILocation(line: 38, column: 14, scope: !344, inlinedAt: !355)
!364 = !DILocation(line: 39, column: 16, scope: !344, inlinedAt: !355)
!365 = !DILocation(line: 39, column: 14, scope: !344, inlinedAt: !355)
!366 = !DILocation(line: 40, column: 16, scope: !344, inlinedAt: !355)
!367 = !DILocation(line: 40, column: 14, scope: !344, inlinedAt: !355)
!368 = !DILocation(line: 41, column: 14, scope: !344, inlinedAt: !355)
!369 = !DILocation(line: 42, column: 14, scope: !344, inlinedAt: !355)
!370 = !DILocation(line: 43, column: 14, scope: !344, inlinedAt: !355)
!371 = !DILocation(line: 95, column: 39, scope: !161)
!372 = !{!250, !216, i64 12}
!373 = !DILocation(line: 95, column: 24, scope: !161)
!374 = !DILocation(line: 96, column: 35, scope: !161)
!375 = !{!250, !216, i64 16}
!376 = !DILocation(line: 96, column: 28, scope: !161)
!377 = !DILocation(line: 97, column: 28, scope: !161)
!378 = !DILocation(line: 99, column: 23, scope: !161)
!379 = !DILocation(line: 99, column: 32, scope: !161)
!380 = !{!267, !217, i64 4}
!381 = !DILocation(line: 102, column: 37, scope: !161)
!382 = !DILocation(line: 102, column: 49, scope: !161)
!383 = !DILocation(line: 102, column: 42, scope: !161)
!384 = !DILocation(line: 102, column: 17, scope: !161)
!385 = !DILocation(line: 108, column: 1, scope: !136)
