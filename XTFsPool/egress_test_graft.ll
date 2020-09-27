; ModuleID = 'egress_test_graft.c'
source_filename = "egress_test_graft.c"
target datalayout = "e-m:e-p:64:64-i64:64-n32:64-S128"
target triple = "bpf"

%struct.bpf_elf_map = type { i32, i32, i32, i32, i32, i32, i32 }
%struct.__sk_buff = type { i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, [5 x i32], i32, i32, i32, i32, i32, i32, i32, i32, [4 x i32], [4 x i32], i32, i32, i32, %union.anon, i64, i32, i32, %union.anon.2 }
%union.anon = type { %struct.bpf_flow_keys* }
%struct.bpf_flow_keys = type { i16, i16, i16, i8, i8, i8, i8, i16, i16, i16, %union.anon.0 }
%union.anon.0 = type { %struct.anon.1 }
%struct.anon.1 = type { [4 x i32], [4 x i32] }
%union.anon.2 = type { %struct.bpf_sock* }
%struct.bpf_sock = type { i32, i32, i32, i32, i32, i32, i32, [4 x i32], i32, i32, i32, [4 x i32], i32 }
%struct.ethhdr = type { [6 x i8], [6 x i8], i16 }
%struct.iphdr = type { i8, i8, i16, i16, i16, i8, i8, i16, i32, i32 }
%struct.vtl_hdr_t = type { i8, i32, i64, i16, i16 }
%struct.sock_state_t = type { i32, i32 }
%struct.vtl_pkt_t = type { [1082 x i8] }
%struct.xdp_md = type { i32, i32, i32, i32, i32 }

@ACK_WND_MAP = global %struct.bpf_elf_map { i32 2, i32 4, i32 8, i32 16, i32 0, i32 0, i32 2 }, section "maps", align 4, !dbg !0
@NUM_SEQ_MAP = global %struct.bpf_elf_map { i32 2, i32 4, i32 2, i32 1, i32 0, i32 0, i32 2 }, section "maps", align 4, !dbg !100
@BUF_WND_MAP = global %struct.bpf_elf_map { i32 2, i32 4, i32 1082, i32 16, i32 0, i32 0, i32 2 }, section "maps", align 4, !dbg !112
@LEN_WND_MAP = global %struct.bpf_elf_map { i32 2, i32 4, i32 2, i32 16, i32 0, i32 0, i32 2 }, section "maps", align 4, !dbg !114
@_license = global [4 x i8] c"GPL\00", section "license", align 1, !dbg !116
@vtl_csum.____fmt = private unnamed_addr constant [35 x i8] c"vtl_csum(): malformed ETH header.\0A\00", align 1
@vtl_csum.____fmt.1 = private unnamed_addr constant [34 x i8] c"vtl_csum(): malformed IP header.\0A\00", align 1
@vtl_csum.____fmt.2 = private unnamed_addr constant [35 x i8] c"vtl_csum(): malformed VTL header.\0A\00", align 1
@vtl_csum.____fmt.3 = private unnamed_addr constant [31 x i8] c"VTL layer: malformed payload.\0A\00", align 1
@llvm.used = appending global [7 x i8*] [i8* bitcast (%struct.bpf_elf_map* @ACK_WND_MAP to i8*), i8* bitcast (%struct.bpf_elf_map* @BUF_WND_MAP to i8*), i8* bitcast (%struct.bpf_elf_map* @LEN_WND_MAP to i8*), i8* bitcast (%struct.bpf_elf_map* @NUM_SEQ_MAP to i8*), i8* getelementptr inbounds ([4 x i8], [4 x i8]* @_license, i32 0, i32 0), i8* bitcast (i32 (%struct.xdp_md*)* @_listener_tf to i8*), i8* bitcast (i32 (%struct.__sk_buff*)* @_tf_tc_egress to i8*)], section "llvm.metadata"

; Function Attrs: nounwind
define i32 @_tf_tc_egress(%struct.__sk_buff*) #0 section "egress_tf_sec" !dbg !166 {
  %2 = alloca [35 x i8], align 1
  call void @llvm.dbg.declare(metadata [35 x i8]* %2, metadata !290, metadata !DIExpression()), !dbg !332
  %3 = alloca [34 x i8], align 1
  call void @llvm.dbg.declare(metadata [34 x i8]* %3, metadata !305, metadata !DIExpression()), !dbg !335
  %4 = alloca [35 x i8], align 1
  call void @llvm.dbg.declare(metadata [35 x i8]* %4, metadata !313, metadata !DIExpression()), !dbg !336
  %5 = alloca [31 x i8], align 1
  call void @llvm.dbg.declare(metadata [31 x i8]* %5, metadata !321, metadata !DIExpression()), !dbg !337
  %6 = alloca i16, align 2
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  call void @llvm.dbg.value(metadata %struct.__sk_buff* %0, metadata !258, metadata !DIExpression()), !dbg !338
  %9 = getelementptr inbounds %struct.__sk_buff, %struct.__sk_buff* %0, i64 0, i32 15, !dbg !339
  %10 = load i32, i32* %9, align 4, !dbg !339, !tbaa !340
  %11 = zext i32 %10 to i64, !dbg !346
  %12 = inttoptr i64 %11 to i8*, !dbg !347
  call void @llvm.dbg.value(metadata i8* %12, metadata !259, metadata !DIExpression()), !dbg !348
  %13 = getelementptr inbounds %struct.__sk_buff, %struct.__sk_buff* %0, i64 0, i32 16, !dbg !349
  %14 = load i32, i32* %13, align 8, !dbg !349, !tbaa !350
  %15 = zext i32 %14 to i64, !dbg !351
  %16 = inttoptr i64 %15 to i8*, !dbg !352
  call void @llvm.dbg.value(metadata i8* %16, metadata !260, metadata !DIExpression()), !dbg !353
  %17 = inttoptr i64 %11 to %struct.ethhdr*, !dbg !354
  call void @llvm.dbg.value(metadata %struct.ethhdr* %17, metadata !261, metadata !DIExpression()), !dbg !355
  %18 = getelementptr inbounds %struct.ethhdr, %struct.ethhdr* %17, i64 1, !dbg !356
  %19 = inttoptr i64 %15 to %struct.ethhdr*, !dbg !358
  %20 = icmp ugt %struct.ethhdr* %18, %19, !dbg !359
  br i1 %20, label %220, label %21, !dbg !360

; <label>:21:                                     ; preds = %1
  call void @llvm.dbg.value(metadata %struct.ethhdr* %18, metadata !262, metadata !DIExpression()), !dbg !361
  %22 = getelementptr inbounds %struct.ethhdr, %struct.ethhdr* %17, i64 2, i32 1, !dbg !362
  %23 = bitcast [6 x i8]* %22 to %struct.iphdr*, !dbg !362
  %24 = inttoptr i64 %15 to %struct.iphdr*, !dbg !364
  %25 = icmp ugt %struct.iphdr* %23, %24, !dbg !365
  br i1 %25, label %220, label %26, !dbg !366

; <label>:26:                                     ; preds = %21
  %27 = getelementptr inbounds %struct.ethhdr, %struct.ethhdr* %17, i64 1, i32 1, i64 3, !dbg !367
  %28 = load i8, i8* %27, align 1, !dbg !367, !tbaa !369
  %29 = icmp eq i8 %28, -3, !dbg !372
  br i1 %29, label %30, label %220, !dbg !373

; <label>:30:                                     ; preds = %26
  call void @llvm.dbg.value(metadata [6 x i8]* %22, metadata !263, metadata !DIExpression()), !dbg !374
  %31 = getelementptr inbounds [6 x i8], [6 x i8]* %22, i64 4, !dbg !375
  %32 = bitcast [6 x i8]* %31 to %struct.vtl_hdr_t*, !dbg !375
  %33 = inttoptr i64 %15 to %struct.vtl_hdr_t*, !dbg !377
  %34 = icmp ugt %struct.vtl_hdr_t* %32, %33, !dbg !378
  br i1 %34, label %220, label %35, !dbg !379

; <label>:35:                                     ; preds = %30
  call void @llvm.dbg.value(metadata %struct.sock_state_t* null, metadata !264, metadata !DIExpression()), !dbg !380
  call void @llvm.dbg.value(metadata %struct.vtl_pkt_t* null, metadata !265, metadata !DIExpression()), !dbg !381
  %36 = bitcast i16* %6 to i8*, !dbg !382
  call void @llvm.lifetime.start.p0i8(i64 2, i8* nonnull %36) #3, !dbg !382
  call void @llvm.dbg.value(metadata i16 0, metadata !266, metadata !DIExpression()), !dbg !383
  store i16 0, i16* %6, align 2, !dbg !383, !tbaa !384
  %37 = bitcast i32* %7 to i8*, !dbg !385
  call void @llvm.lifetime.start.p0i8(i64 4, i8* nonnull %37) #3, !dbg !385
  call void @llvm.dbg.value(metadata i32 0, metadata !267, metadata !DIExpression()), !dbg !386
  store i32 0, i32* %7, align 4, !dbg !386, !tbaa !387
  %38 = bitcast i32* %8 to i8*, !dbg !385
  call void @llvm.lifetime.start.p0i8(i64 4, i8* nonnull %38) #3, !dbg !385
  call void @llvm.dbg.value(metadata i32 0, metadata !268, metadata !DIExpression()), !dbg !388
  store i32 0, i32* %8, align 4, !dbg !388, !tbaa !387
  %39 = getelementptr inbounds %struct.__sk_buff, %struct.__sk_buff* %0, i64 0, i32 0, !dbg !389
  %40 = load i32, i32* %39, align 8, !dbg !389, !tbaa !390
  call void @llvm.dbg.value(metadata i16* null, metadata !271, metadata !DIExpression()), !dbg !391
  %41 = getelementptr inbounds %struct.ethhdr, %struct.ethhdr* %17, i64 2, i32 1, i64 4, !dbg !392
  %42 = bitcast i8* %41 to i32*, !dbg !392
  store i32 1, i32* %42, align 4, !dbg !393, !tbaa !394
  call void @llvm.dbg.value(metadata i16 0, metadata !272, metadata !DIExpression()), !dbg !397
  call void @llvm.dbg.value(metadata i8* %12, metadata !298, metadata !DIExpression()) #3, !dbg !398
  call void @llvm.dbg.value(metadata i8* %16, metadata !299, metadata !DIExpression()) #3, !dbg !399
  call void @llvm.dbg.value(metadata i32 0, metadata !301, metadata !DIExpression()) #3, !dbg !400
  call void @llvm.dbg.value(metadata i8 0, metadata !302, metadata !DIExpression()) #3, !dbg !401
  call void @llvm.dbg.value(metadata i8* %12, metadata !303, metadata !DIExpression()) #3, !dbg !402
  %43 = getelementptr inbounds i8, i8* %12, i64 14, !dbg !403
  %44 = icmp ugt i8* %43, %16, !dbg !404
  br i1 %44, label %45, label %48, !dbg !405

; <label>:45:                                     ; preds = %35
  %46 = getelementptr inbounds [35 x i8], [35 x i8]* %2, i64 0, i64 0, !dbg !332
  call void @llvm.lifetime.start.p0i8(i64 35, i8* nonnull %46) #3, !dbg !332
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* nonnull %46, i8* getelementptr inbounds ([35 x i8], [35 x i8]* @vtl_csum.____fmt, i64 0, i64 0), i64 35, i32 1, i1 false) #3, !dbg !332
  %47 = call i32 (i8*, i32, ...) inttoptr (i64 7 to i32 (i8*, i32, ...)*)(i8* nonnull %46, i32 35) #3, !dbg !332
  call void @llvm.lifetime.end.p0i8(i64 35, i8* nonnull %46) #3, !dbg !406
  br label %218, !dbg !407

; <label>:48:                                     ; preds = %35
  call void @llvm.dbg.value(metadata i8* %43, metadata !304, metadata !DIExpression()) #3, !dbg !408
  %49 = getelementptr inbounds i8, i8* %12, i64 34, !dbg !409
  %50 = icmp ugt i8* %49, %16, !dbg !410
  br i1 %50, label %51, label %54, !dbg !411

; <label>:51:                                     ; preds = %48
  %52 = getelementptr inbounds [34 x i8], [34 x i8]* %3, i64 0, i64 0, !dbg !335
  call void @llvm.lifetime.start.p0i8(i64 34, i8* nonnull %52) #3, !dbg !335
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* nonnull %52, i8* getelementptr inbounds ([34 x i8], [34 x i8]* @vtl_csum.____fmt.1, i64 0, i64 0), i64 34, i32 1, i1 false) #3, !dbg !335
  %53 = call i32 (i8*, i32, ...) inttoptr (i64 7 to i32 (i8*, i32, ...)*)(i8* nonnull %52, i32 34) #3, !dbg !335
  call void @llvm.lifetime.end.p0i8(i64 34, i8* nonnull %52) #3, !dbg !412
  br label %218, !dbg !413

; <label>:54:                                     ; preds = %48
  %55 = getelementptr inbounds i8, i8* %12, i64 23, !dbg !414
  %56 = load i8, i8* %55, align 1, !dbg !414, !tbaa !369
  %57 = icmp eq i8 %56, -3, !dbg !416
  br i1 %57, label %58, label %218, !dbg !417

; <label>:58:                                     ; preds = %54
  call void @llvm.dbg.value(metadata i8* %49, metadata !312, metadata !DIExpression()) #3, !dbg !418
  %59 = getelementptr inbounds i8, i8* %12, i64 58, !dbg !419
  %60 = icmp ugt i8* %59, %16, !dbg !420
  br i1 %60, label %61, label %64, !dbg !421

; <label>:61:                                     ; preds = %58
  %62 = getelementptr inbounds [35 x i8], [35 x i8]* %4, i64 0, i64 0, !dbg !336
  call void @llvm.lifetime.start.p0i8(i64 35, i8* nonnull %62) #3, !dbg !336
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* nonnull %62, i8* getelementptr inbounds ([35 x i8], [35 x i8]* @vtl_csum.____fmt.2, i64 0, i64 0), i64 35, i32 1, i1 false) #3, !dbg !336
  %63 = call i32 (i8*, i32, ...) inttoptr (i64 7 to i32 (i8*, i32, ...)*)(i8* nonnull %62, i32 35) #3, !dbg !336
  call void @llvm.lifetime.end.p0i8(i64 35, i8* nonnull %62) #3, !dbg !422
  br label %218, !dbg !423

; <label>:64:                                     ; preds = %58
  call void @llvm.dbg.value(metadata i8* %59, metadata !320, metadata !DIExpression()) #3, !dbg !424
  %65 = getelementptr inbounds i8, i8* %12, i64 59, !dbg !425
  %66 = icmp ugt i8* %65, %16, !dbg !426
  br i1 %66, label %67, label %70, !dbg !427

; <label>:67:                                     ; preds = %64
  %68 = getelementptr inbounds [31 x i8], [31 x i8]* %5, i64 0, i64 0, !dbg !337
  call void @llvm.lifetime.start.p0i8(i64 31, i8* nonnull %68) #3, !dbg !337
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* nonnull %68, i8* getelementptr inbounds ([31 x i8], [31 x i8]* @vtl_csum.____fmt.3, i64 0, i64 0), i64 31, i32 1, i1 false) #3, !dbg !337
  %69 = call i32 (i8*, i32, ...) inttoptr (i64 7 to i32 (i8*, i32, ...)*)(i8* nonnull %68, i32 31) #3, !dbg !337
  call void @llvm.lifetime.end.p0i8(i64 31, i8* nonnull %68) #3, !dbg !428
  br label %218, !dbg !429

; <label>:70:                                     ; preds = %64
  call void @llvm.dbg.value(metadata i32 0, metadata !301, metadata !DIExpression()) #3, !dbg !400
  call void @llvm.dbg.value(metadata i8 0, metadata !302, metadata !DIExpression()) #3, !dbg !401
  %71 = getelementptr inbounds i8, i8* %12, i64 42, !dbg !430
  %72 = bitcast i8* %71 to i64*, !dbg !430
  %73 = load i64, i64* %72, align 8, !dbg !430, !tbaa !431
  %74 = icmp eq i64 %73, 0, !dbg !432
  br i1 %74, label %92, label %75, !dbg !433

; <label>:75:                                     ; preds = %70
  br label %76, !dbg !434

; <label>:76:                                     ; preds = %75, %83
  %77 = phi i64 [ %86, %83 ], [ 0, %75 ]
  %78 = phi i8 [ %85, %83 ], [ 0, %75 ]
  call void @llvm.dbg.value(metadata i8 %78, metadata !302, metadata !DIExpression()) #3, !dbg !401
  call void @llvm.dbg.value(metadata i64 %77, metadata !301, metadata !DIExpression()) #3, !dbg !400
  %79 = getelementptr inbounds i8, i8* %59, i64 %77, !dbg !434
  %80 = getelementptr inbounds i8, i8* %79, i64 -1, !dbg !435
  call void @llvm.dbg.value(metadata i8* %80, metadata !328, metadata !DIExpression()) #3, !dbg !436
  %81 = getelementptr inbounds i8, i8* %80, i64 1, !dbg !437
  %82 = icmp ugt i8* %81, %16, !dbg !439
  br i1 %82, label %218, label %83, !dbg !440

; <label>:83:                                     ; preds = %76
  %84 = load i8, i8* %80, align 1, !dbg !441, !tbaa !442
  %85 = xor i8 %84, %78, !dbg !443
  %86 = add nuw nsw i64 %77, 1, !dbg !444
  call void @llvm.dbg.value(metadata i8 %85, metadata !302, metadata !DIExpression()) #3, !dbg !401
  %87 = icmp ule i64 %73, %86, !dbg !432
  %88 = icmp ugt i64 %77, 1023, !dbg !445
  %89 = or i1 %88, %87, !dbg !433
  br i1 %89, label %90, label %76, !dbg !433, !llvm.loop !447

; <label>:90:                                     ; preds = %83
  %91 = zext i8 %85 to i16, !dbg !450
  br label %92, !dbg !450

; <label>:92:                                     ; preds = %90, %70
  %93 = phi i16 [ 0, %70 ], [ %91, %90 ]
  call void @llvm.dbg.value(metadata i16 %93, metadata !272, metadata !DIExpression()), !dbg !397
  %94 = getelementptr inbounds [6 x i8], [6 x i8]* %22, i64 2, i64 4, !dbg !451
  %95 = bitcast i8* %94 to i16*, !dbg !451
  store i16 %93, i16* %95, align 8, !dbg !452, !tbaa !453
  %96 = call i8* inttoptr (i64 1 to i8* (i8*, i8*)*)(i8* bitcast (%struct.bpf_elf_map* @NUM_SEQ_MAP to i8*), i8* nonnull %37) #3, !dbg !454
  %97 = bitcast i8* %96 to i16*, !dbg !455
  call void @llvm.dbg.value(metadata i16* %97, metadata !271, metadata !DIExpression()), !dbg !391
  %98 = icmp eq i8* %96, null, !dbg !456
  br i1 %98, label %218, label %99, !dbg !458

; <label>:99:                                     ; preds = %92
  %100 = load i16, i16* %97, align 2, !dbg !459, !tbaa !384
  %101 = getelementptr inbounds [6 x i8], [6 x i8]* %22, i64 3, !dbg !460
  %102 = bitcast [6 x i8]* %101 to i16*, !dbg !460
  store i16 %100, i16* %102, align 2, !dbg !461, !tbaa !462
  %103 = load i16, i16* %97, align 2, !dbg !463, !tbaa !384
  %104 = zext i16 %103 to i32, !dbg !464
  call void @llvm.dbg.value(metadata i32 %104, metadata !268, metadata !DIExpression()), !dbg !388
  store i32 %104, i32* %8, align 4, !dbg !465, !tbaa !387
  %105 = call i8* inttoptr (i64 1 to i8* (i8*, i8*)*)(i8* bitcast (%struct.bpf_elf_map* @ACK_WND_MAP to i8*), i8* nonnull %38) #3, !dbg !466
  call void @llvm.dbg.value(metadata i8* %105, metadata !264, metadata !DIExpression()), !dbg !380
  %106 = icmp eq i8* %105, null, !dbg !467
  br i1 %106, label %218, label %107, !dbg !469

; <label>:107:                                    ; preds = %99
  %108 = getelementptr inbounds i8, i8* %105, i64 4, !dbg !470
  %109 = bitcast i8* %108 to i32*, !dbg !470
  store i32 2, i32* %109, align 1, !dbg !471, !tbaa !472
  %110 = call i32 inttoptr (i64 2 to i32 (i8*, i8*, i8*, i64)*)(i8* bitcast (%struct.bpf_elf_map* @ACK_WND_MAP to i8*), i8* nonnull %38, i8* nonnull %105, i64 0) #3, !dbg !474
  %111 = getelementptr inbounds [6 x i8], [6 x i8]* %22, i64 1, i64 2, !dbg !475
  %112 = bitcast i8* %111 to i64*, !dbg !475
  %113 = load i64, i64* %112, align 8, !dbg !475, !tbaa !431
  %114 = trunc i64 %113 to i32, !dbg !476
  call void @llvm.dbg.value(metadata i32 %114, metadata !273, metadata !DIExpression()), !dbg !477
  %115 = icmp eq i32 %114, 1024, !dbg !478
  br i1 %115, label %135, label %116, !dbg !480

; <label>:116:                                    ; preds = %107
  %117 = load i32, i32* %39, align 8, !dbg !481, !tbaa !390
  %118 = trunc i32 %117 to i16, !dbg !483
  call void @llvm.dbg.value(metadata i16 %118, metadata !266, metadata !DIExpression()), !dbg !383
  store i16 %118, i16* %6, align 2, !dbg !484, !tbaa !384
  %119 = call i32 inttoptr (i64 2 to i32 (i8*, i8*, i8*, i64)*)(i8* bitcast (%struct.bpf_elf_map* @LEN_WND_MAP to i8*), i8* nonnull %38, i8* nonnull %36, i64 0) #3, !dbg !485
  %120 = bitcast %struct.__sk_buff* %0 to i8*, !dbg !486
  %121 = call i32 inttoptr (i64 41 to i32 (i8*, i32, i64)*)(i8* %120, i32 1082, i64 0) #3, !dbg !487
  %122 = load i32, i32* %9, align 4, !dbg !488, !tbaa !340
  %123 = zext i32 %122 to i64, !dbg !489
  call void @llvm.dbg.value(metadata i8* %131, metadata !259, metadata !DIExpression()), !dbg !348
  %124 = load i32, i32* %13, align 8, !dbg !490, !tbaa !350
  %125 = zext i32 %124 to i64, !dbg !491
  call void @llvm.dbg.value(metadata i64 %125, metadata !260, metadata !DIExpression()), !dbg !353
  %126 = inttoptr i64 %123 to %struct.vtl_pkt_t*, !dbg !492
  call void @llvm.dbg.value(metadata %struct.vtl_pkt_t* %126, metadata !265, metadata !DIExpression()), !dbg !381
  %127 = getelementptr inbounds %struct.vtl_pkt_t, %struct.vtl_pkt_t* %126, i64 1, !dbg !493
  %128 = inttoptr i64 %125 to %struct.vtl_pkt_t*, !dbg !495
  %129 = icmp ugt %struct.vtl_pkt_t* %127, %128, !dbg !496
  br i1 %129, label %218, label %130, !dbg !497

; <label>:130:                                    ; preds = %116
  %131 = inttoptr i64 %123 to i8*, !dbg !498
  %132 = call i32 inttoptr (i64 2 to i32 (i8*, i8*, i8*, i64)*)(i8* bitcast (%struct.bpf_elf_map* @BUF_WND_MAP to i8*), i8* nonnull %38, i8* %131, i64 0) #3, !dbg !499
  %133 = and i32 %40, 65535, !dbg !500
  %134 = call i32 inttoptr (i64 41 to i32 (i8*, i32, i64)*)(i8* %120, i32 %133, i64 0) #3, !dbg !501
  br label %144, !dbg !502

; <label>:135:                                    ; preds = %107
  call void @llvm.dbg.value(metadata i16 1082, metadata !266, metadata !DIExpression()), !dbg !383
  store i16 1082, i16* %6, align 2, !dbg !503, !tbaa !384
  %136 = call i32 inttoptr (i64 2 to i32 (i8*, i8*, i8*, i64)*)(i8* bitcast (%struct.bpf_elf_map* @LEN_WND_MAP to i8*), i8* nonnull %38, i8* nonnull %36, i64 0) #3, !dbg !505
  %137 = inttoptr i64 %11 to %struct.vtl_pkt_t*, !dbg !506
  call void @llvm.dbg.value(metadata %struct.vtl_pkt_t* %137, metadata !265, metadata !DIExpression()), !dbg !381
  %138 = getelementptr inbounds %struct.vtl_pkt_t, %struct.vtl_pkt_t* %137, i64 1, !dbg !507
  %139 = inttoptr i64 %15 to %struct.vtl_pkt_t*, !dbg !509
  %140 = icmp ugt %struct.vtl_pkt_t* %138, %139, !dbg !510
  br i1 %140, label %218, label %141, !dbg !511

; <label>:141:                                    ; preds = %135
  %142 = call i32 inttoptr (i64 2 to i32 (i8*, i8*, i8*, i64)*)(i8* bitcast (%struct.bpf_elf_map* @BUF_WND_MAP to i8*), i8* nonnull %38, i8* nonnull %12, i64 0) #3, !dbg !512
  %143 = bitcast %struct.__sk_buff* %0 to i8*, !dbg !513
  br label %144

; <label>:144:                                    ; preds = %141, %130
  %145 = phi i8* [ %143, %141 ], [ %120, %130 ], !dbg !513
  %146 = load i16, i16* %97, align 2, !dbg !514, !tbaa !384
  call void @llvm.dbg.value(metadata i16 %146, metadata !274, metadata !DIExpression()), !dbg !515
  %147 = add i16 %146, 1, !dbg !516
  %148 = and i16 %147, 15, !dbg !517
  store i16 %148, i16* %97, align 2, !dbg !517, !tbaa !384
  %149 = call i32 inttoptr (i64 2 to i32 (i8*, i8*, i8*, i64)*)(i8* bitcast (%struct.bpf_elf_map* @NUM_SEQ_MAP to i8*), i8* nonnull %37, i8* nonnull %96, i64 0) #3, !dbg !518
  %150 = getelementptr inbounds %struct.__sk_buff, %struct.__sk_buff* %0, i64 0, i32 10, !dbg !519
  %151 = load i32, i32* %150, align 8, !dbg !519, !tbaa !520
  %152 = add i32 %151, 1, !dbg !521
  %153 = call i32 inttoptr (i64 16 to i32 (i8*, i32, i32)*)(i8* %145, i32 %152, i32 0) #3, !dbg !522
  call void @llvm.dbg.value(metadata i32 0, metadata !275, metadata !DIExpression()), !dbg !523
  %154 = load i32, i32* %8, align 4, !dbg !524, !tbaa !387
  call void @llvm.dbg.value(metadata i32 %154, metadata !268, metadata !DIExpression()), !dbg !388
  %155 = icmp eq i32 %154, 15, !dbg !525
  %156 = load i16, i16* %6, align 2, !dbg !526
  %157 = icmp ult i16 %156, 1082, !dbg !527
  %158 = or i1 %155, %157, !dbg !528
  br i1 %158, label %159, label %218, !dbg !528

; <label>:159:                                    ; preds = %144
  call void @llvm.dbg.value(metadata i32 0, metadata !276, metadata !DIExpression()), !dbg !529
  call void @llvm.dbg.value(metadata i32 0, metadata !279, metadata !DIExpression()), !dbg !530
  call void @llvm.dbg.value(metadata i32 0, metadata !280, metadata !DIExpression()), !dbg !531
  call void @llvm.dbg.value(metadata i32 0, metadata !281, metadata !DIExpression()), !dbg !532
  call void @llvm.dbg.value(metadata i32 0, metadata !275, metadata !DIExpression()), !dbg !523
  call void inttoptr (i64 6 to void (i64)*)(i64 10) #3, !dbg !533
  call void @llvm.dbg.value(metadata i32 1, metadata !275, metadata !DIExpression()), !dbg !523
  %160 = zext i16 %146 to i32
  %161 = and i32 %40, 65535
  br label %162, !dbg !534

; <label>:162:                                    ; preds = %159, %215
  %163 = phi i32 [ 1, %159 ], [ %216, %215 ]
  %164 = phi i32 [ 0, %159 ], [ %170, %215 ]
  call void @llvm.dbg.value(metadata i32 %164, metadata !279, metadata !DIExpression()), !dbg !530
  call void @llvm.dbg.value(metadata i32 %164, metadata !276, metadata !DIExpression()), !dbg !529
  %165 = icmp ugt i32 %164, 15, !dbg !535
  %166 = icmp ugt i32 %164, %160, !dbg !538
  %167 = or i1 %165, %166, !dbg !541
  br i1 %167, label %218, label %168, !dbg !541

; <label>:168:                                    ; preds = %162
  br label %169, !dbg !542

; <label>:169:                                    ; preds = %168, %178
  %170 = phi i32 [ %179, %178 ], [ %164, %168 ]
  call void @llvm.dbg.value(metadata i32 %170, metadata !268, metadata !DIExpression()), !dbg !388
  call void @llvm.dbg.value(metadata i32 %170, metadata !279, metadata !DIExpression()), !dbg !530
  store i32 %170, i32* %8, align 4, !dbg !542, !tbaa !387
  %171 = call i8* inttoptr (i64 1 to i8* (i8*, i8*)*)(i8* bitcast (%struct.bpf_elf_map* @ACK_WND_MAP to i8*), i8* nonnull %38) #3, !dbg !543
  call void @llvm.dbg.value(metadata i8* %171, metadata !264, metadata !DIExpression()), !dbg !380
  %172 = icmp eq i8* %171, null, !dbg !544
  br i1 %172, label %218, label %173, !dbg !546

; <label>:173:                                    ; preds = %169
  %174 = getelementptr inbounds i8, i8* %171, i64 4, !dbg !547
  %175 = bitcast i8* %174 to i32*, !dbg !547
  %176 = load i32, i32* %175, align 1, !dbg !547, !tbaa !472
  %177 = icmp eq i32 %176, 0, !dbg !549
  br i1 %177, label %178, label %183, !dbg !550

; <label>:178:                                    ; preds = %173
  %179 = add nuw nsw i32 %170, 1, !dbg !551
  call void @llvm.dbg.value(metadata i32 %179, metadata !279, metadata !DIExpression()), !dbg !530
  %180 = icmp ugt i32 %170, 14, !dbg !535
  %181 = icmp uge i32 %170, %160, !dbg !538
  %182 = or i1 %180, %181, !dbg !541
  br i1 %182, label %218, label %169, !dbg !541, !llvm.loop !552

; <label>:183:                                    ; preds = %173
  call void @llvm.dbg.value(metadata i32 undef, metadata !276, metadata !DIExpression()), !dbg !529
  call void @llvm.dbg.value(metadata i32 undef, metadata !280, metadata !DIExpression()), !dbg !531
  call void @llvm.dbg.value(metadata i32 %170, metadata !280, metadata !DIExpression()), !dbg !531
  %184 = icmp ugt i32 %170, 15, !dbg !554
  %185 = icmp ugt i32 %170, %160, !dbg !555
  %186 = or i1 %184, %185, !dbg !557
  br i1 %186, label %215, label %187, !dbg !557

; <label>:187:                                    ; preds = %183
  br label %188, !dbg !558

; <label>:188:                                    ; preds = %187, %207
  %189 = phi i32 [ %211, %207 ], [ %170, %187 ]
  call void @llvm.dbg.value(metadata i32 %189, metadata !268, metadata !DIExpression()), !dbg !388
  call void @llvm.dbg.value(metadata i32 %189, metadata !280, metadata !DIExpression()), !dbg !531
  store i32 %189, i32* %8, align 4, !dbg !558, !tbaa !387
  %190 = call i8* inttoptr (i64 1 to i8* (i8*, i8*)*)(i8* bitcast (%struct.bpf_elf_map* @BUF_WND_MAP to i8*), i8* nonnull %38) #3, !dbg !559
  call void @llvm.dbg.value(metadata i8* %190, metadata !282, metadata !DIExpression()), !dbg !560
  %191 = icmp eq i8* %190, null, !dbg !561
  br i1 %191, label %215, label %192, !dbg !563

; <label>:192:                                    ; preds = %188
  %193 = call i8* inttoptr (i64 1 to i8* (i8*, i8*)*)(i8* bitcast (%struct.bpf_elf_map* @LEN_WND_MAP to i8*), i8* nonnull %38) #3, !dbg !564
  %194 = bitcast i8* %193 to i16*, !dbg !565
  call void @llvm.dbg.value(metadata i16* %194, metadata !289, metadata !DIExpression()), !dbg !566
  %195 = icmp eq i8* %193, null, !dbg !567
  br i1 %195, label %215, label %196, !dbg !569

; <label>:196:                                    ; preds = %192
  %197 = load i16, i16* %194, align 2, !dbg !570, !tbaa !384
  %198 = icmp eq i16 %197, 1082, !dbg !572
  br i1 %198, label %199, label %201, !dbg !573

; <label>:199:                                    ; preds = %196
  %200 = call i32 inttoptr (i64 41 to i32 (i8*, i32, i64)*)(i8* %145, i32 1082, i64 0) #3, !dbg !574
  br label %201, !dbg !574

; <label>:201:                                    ; preds = %199, %196
  %202 = call i32 inttoptr (i64 11 to i32 (i8*, i32, i8*, i32, i32)*)(i8* %145, i32 0, i8* nonnull %190, i32 1082, i32 0) #3, !dbg !575
  %203 = load i16, i16* %194, align 2, !dbg !576, !tbaa !384
  %204 = icmp ult i16 %203, 1082, !dbg !578
  br i1 %204, label %205, label %207, !dbg !579

; <label>:205:                                    ; preds = %201
  %206 = call i32 inttoptr (i64 41 to i32 (i8*, i32, i64)*)(i8* %145, i32 %161, i64 0) #3, !dbg !580
  br label %207, !dbg !580

; <label>:207:                                    ; preds = %205, %201
  %208 = load i32, i32* %150, align 8, !dbg !581, !tbaa !520
  %209 = add i32 %208, 1, !dbg !582
  %210 = call i32 inttoptr (i64 16 to i32 (i8*, i32, i32)*)(i8* %145, i32 %209, i32 0) #3, !dbg !583
  %211 = add nuw nsw i32 %189, 1, !dbg !584
  call void @llvm.dbg.value(metadata i32 %211, metadata !280, metadata !DIExpression()), !dbg !531
  %212 = icmp ugt i32 %189, 14, !dbg !554
  %213 = icmp uge i32 %189, %160, !dbg !555
  %214 = or i1 %212, %213, !dbg !557
  br i1 %214, label %215, label %188, !dbg !557, !llvm.loop !585

; <label>:215:                                    ; preds = %207, %188, %192, %183
  call void @llvm.dbg.value(metadata i32 0, metadata !281, metadata !DIExpression()), !dbg !532
  call void @llvm.dbg.value(metadata i32 0, metadata !281, metadata !DIExpression()), !dbg !532
  call void @llvm.dbg.value(metadata i32 %170, metadata !276, metadata !DIExpression()), !dbg !529
  call void @llvm.dbg.value(metadata i32 %163, metadata !275, metadata !DIExpression()), !dbg !523
  call void inttoptr (i64 6 to void (i64)*)(i64 10) #3, !dbg !533
  %216 = add nuw nsw i32 %163, 1, !dbg !587
  call void @llvm.dbg.value(metadata i32 %216, metadata !275, metadata !DIExpression()), !dbg !523
  %217 = icmp ugt i32 %163, 9, !dbg !588
  br i1 %217, label %218, label %162, !dbg !534

; <label>:218:                                    ; preds = %76, %215, %162, %169, %178, %67, %61, %54, %51, %45, %116, %135, %144, %99, %92
  %219 = phi i32 [ 0, %92 ], [ 0, %99 ], [ 0, %116 ], [ 0, %135 ], [ 2, %144 ], [ 0, %45 ], [ 0, %51 ], [ 0, %54 ], [ 0, %61 ], [ 0, %67 ], [ 2, %178 ], [ 2, %169 ], [ 2, %162 ], [ 2, %215 ], [ 0, %76 ]
  call void @llvm.lifetime.end.p0i8(i64 4, i8* nonnull %38) #3, !dbg !590
  call void @llvm.lifetime.end.p0i8(i64 4, i8* nonnull %37) #3, !dbg !590
  call void @llvm.lifetime.end.p0i8(i64 2, i8* nonnull %36) #3, !dbg !590
  br label %220

; <label>:220:                                    ; preds = %21, %26, %30, %218, %1
  %221 = phi i32 [ 0, %1 ], [ 0, %21 ], [ 0, %26 ], [ %219, %218 ], [ 0, %30 ]
  ret i32 %221, !dbg !590
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.start.p0i8(i64, i8* nocapture) #2

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.end.p0i8(i64, i8* nocapture) #2

; Function Attrs: nounwind
define i32 @_listener_tf(%struct.xdp_md* nocapture readonly) #0 section "listener_tf_sec" !dbg !591 {
  %2 = alloca i32, align 4
  call void @llvm.dbg.value(metadata %struct.xdp_md* %0, metadata !603, metadata !DIExpression()), !dbg !612
  %3 = bitcast i32* %2 to i8*, !dbg !613
  call void @llvm.lifetime.start.p0i8(i64 4, i8* nonnull %3) #3, !dbg !613
  call void @llvm.dbg.value(metadata i32 0, metadata !604, metadata !DIExpression()), !dbg !614
  store i32 0, i32* %2, align 4, !dbg !614, !tbaa !387
  call void @llvm.dbg.value(metadata %struct.sock_state_t* null, metadata !605, metadata !DIExpression()), !dbg !615
  %4 = getelementptr inbounds %struct.xdp_md, %struct.xdp_md* %0, i64 0, i32 0, !dbg !616
  %5 = load i32, i32* %4, align 4, !dbg !616, !tbaa !617
  %6 = zext i32 %5 to i64, !dbg !619
  call void @llvm.dbg.value(metadata i64 %6, metadata !606, metadata !DIExpression()), !dbg !620
  %7 = getelementptr inbounds %struct.xdp_md, %struct.xdp_md* %0, i64 0, i32 1, !dbg !621
  %8 = load i32, i32* %7, align 4, !dbg !621, !tbaa !622
  %9 = zext i32 %8 to i64, !dbg !623
  call void @llvm.dbg.value(metadata i64 %9, metadata !607, metadata !DIExpression()), !dbg !624
  %10 = inttoptr i64 %6 to %struct.ethhdr*, !dbg !625
  call void @llvm.dbg.value(metadata %struct.ethhdr* %10, metadata !608, metadata !DIExpression()), !dbg !626
  %11 = getelementptr inbounds %struct.ethhdr, %struct.ethhdr* %10, i64 1, !dbg !627
  %12 = inttoptr i64 %9 to %struct.ethhdr*, !dbg !629
  %13 = icmp ugt %struct.ethhdr* %11, %12, !dbg !630
  br i1 %13, label %44, label %14, !dbg !631

; <label>:14:                                     ; preds = %1
  call void @llvm.dbg.value(metadata %struct.ethhdr* %11, metadata !609, metadata !DIExpression()), !dbg !632
  %15 = getelementptr inbounds %struct.ethhdr, %struct.ethhdr* %10, i64 2, i32 1, !dbg !633
  %16 = bitcast [6 x i8]* %15 to %struct.iphdr*, !dbg !633
  %17 = inttoptr i64 %9 to %struct.iphdr*, !dbg !635
  %18 = icmp ugt %struct.iphdr* %16, %17, !dbg !636
  br i1 %18, label %44, label %19, !dbg !637

; <label>:19:                                     ; preds = %14
  %20 = getelementptr inbounds %struct.ethhdr, %struct.ethhdr* %10, i64 1, i32 1, i64 3, !dbg !638
  %21 = load i8, i8* %20, align 1, !dbg !638, !tbaa !369
  %22 = icmp eq i8 %21, -3, !dbg !640
  br i1 %22, label %23, label %44, !dbg !641

; <label>:23:                                     ; preds = %19
  call void @llvm.dbg.value(metadata [6 x i8]* %15, metadata !610, metadata !DIExpression()), !dbg !642
  %24 = getelementptr inbounds [6 x i8], [6 x i8]* %15, i64 4, !dbg !643
  %25 = bitcast [6 x i8]* %24 to %struct.vtl_hdr_t*, !dbg !643
  %26 = inttoptr i64 %9 to %struct.vtl_hdr_t*, !dbg !645
  %27 = icmp ugt %struct.vtl_hdr_t* %25, %26, !dbg !646
  br i1 %27, label %44, label %28, !dbg !647

; <label>:28:                                     ; preds = %23
  %29 = getelementptr inbounds [6 x i8], [6 x i8]* %15, i64 3, !dbg !648
  %30 = bitcast [6 x i8]* %29 to i16*, !dbg !648
  %31 = load i16, i16* %30, align 2, !dbg !648, !tbaa !462
  %32 = zext i16 %31 to i32, !dbg !649
  call void @llvm.dbg.value(metadata i32 %32, metadata !604, metadata !DIExpression()), !dbg !614
  store i32 %32, i32* %2, align 4, !dbg !650, !tbaa !387
  %33 = call i8* inttoptr (i64 1 to i8* (i8*, i8*)*)(i8* bitcast (%struct.bpf_elf_map* @ACK_WND_MAP to i8*), i8* nonnull %3) #3, !dbg !651
  call void @llvm.dbg.value(metadata i8* %33, metadata !605, metadata !DIExpression()), !dbg !615
  %34 = icmp eq i8* %33, null, !dbg !652
  br i1 %34, label %44, label %35, !dbg !654

; <label>:35:                                     ; preds = %28
  %36 = getelementptr inbounds %struct.ethhdr, %struct.ethhdr* %10, i64 2, i32 1, i64 4, !dbg !655
  %37 = bitcast i8* %36 to i32*, !dbg !655
  %38 = load i32, i32* %37, align 4, !dbg !655, !tbaa !394
  %39 = icmp eq i32 %38, 3, !dbg !657
  %40 = getelementptr inbounds i8, i8* %33, i64 4
  %41 = bitcast i8* %40 to i32*
  %42 = select i1 %39, i32 2, i32 0, !dbg !658
  store i32 %42, i32* %41, align 1, !tbaa !472
  %43 = call i32 inttoptr (i64 2 to i32 (i8*, i8*, i8*, i64)*)(i8* bitcast (%struct.bpf_elf_map* @ACK_WND_MAP to i8*), i8* nonnull %3, i8* nonnull %33, i64 0) #3, !dbg !659
  br label %44

; <label>:44:                                     ; preds = %14, %19, %28, %23, %35, %1
  call void @llvm.lifetime.end.p0i8(i64 4, i8* nonnull %3) #3, !dbg !660
  ret i32 2, !dbg !660
}

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i32, i1) #2

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.value(metadata, metadata, metadata) #1

attributes #0 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { argmemonly nounwind }
attributes #3 = { nounwind }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!162, !163, !164}
!llvm.ident = !{!165}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "ACK_WND_MAP", scope: !2, file: !3, line: 22, type: !102, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 6.0.0-1ubuntu2 (tags/RELEASE_600/final)", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug, enums: !4, retainedTypes: !22, globals: !99)
!3 = !DIFile(filename: "egress_test_graft.c", directory: "/home/vtl_server/vtlWorkspace/vtl_server/XTFsPool")
!4 = !{!5, !14}
!5 = !DICompositeType(tag: DW_TAG_enumeration_type, file: !6, line: 66, size: 32, elements: !7)
!6 = !DIFile(filename: "./../include/vtl.h", directory: "/home/vtl_server/vtlWorkspace/vtl_server/XTFsPool")
!7 = !{!8, !9, !10, !11, !12, !13}
!8 = !DIEnumerator(name: "NEGO", value: 0)
!9 = !DIEnumerator(name: "DATA", value: 1)
!10 = !DIEnumerator(name: "ACK", value: 2)
!11 = !DIEnumerator(name: "NACK", value: 3)
!12 = !DIEnumerator(name: "NEGO_ACK", value: 4)
!13 = !DIEnumerator(name: "NEGO_NACK", value: 5)
!14 = !DICompositeType(tag: DW_TAG_enumeration_type, name: "xdp_action", file: !15, line: 2848, size: 32, elements: !16)
!15 = !DIFile(filename: "./include/linux/bpf.h", directory: "/home/vtl_server/vtlWorkspace/vtl_server/XTFsPool")
!16 = !{!17, !18, !19, !20, !21}
!17 = !DIEnumerator(name: "XDP_ABORTED", value: 0)
!18 = !DIEnumerator(name: "XDP_DROP", value: 1)
!19 = !DIEnumerator(name: "XDP_PASS", value: 2)
!20 = !DIEnumerator(name: "XDP_TX", value: 3)
!21 = !DIEnumerator(name: "XDP_REDIRECT", value: 4)
!22 = !{!23, !24, !25, !41, !65, !54, !82, !46, !83, !62, !90, !89, !98}
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
!66 = !DIDerivedType(tag: DW_TAG_typedef, name: "vtl_hdr_t", file: !6, line: 84, baseType: !67)
!67 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !6, line: 78, size: 192, elements: !68)
!68 = !{!69, !74, !76, !80, !81}
!69 = !DIDerivedType(tag: DW_TAG_member, name: "gid", scope: !67, file: !6, line: 79, baseType: !70, size: 8)
!70 = !DIDerivedType(tag: DW_TAG_typedef, name: "int8_t", file: !71, line: 24, baseType: !72)
!71 = !DIFile(filename: "/usr/include/bits/stdint-intn.h", directory: "/home/vtl_server/vtlWorkspace/vtl_server/XTFsPool")
!72 = !DIDerivedType(tag: DW_TAG_typedef, name: "__int8_t", file: !52, line: 36, baseType: !73)
!73 = !DIBasicType(name: "signed char", size: 8, encoding: DW_ATE_signed_char)
!74 = !DIDerivedType(tag: DW_TAG_member, name: "pkt_type", scope: !67, file: !6, line: 80, baseType: !75, size: 32, offset: 32)
!75 = !DIDerivedType(tag: DW_TAG_typedef, name: "vtl_pkt_type", file: !6, line: 73, baseType: !5)
!76 = !DIDerivedType(tag: DW_TAG_member, name: "payload_len", scope: !67, file: !6, line: 81, baseType: !77, size: 64, offset: 64)
!77 = !DIDerivedType(tag: DW_TAG_typedef, name: "size_t", file: !78, line: 62, baseType: !79)
!78 = !DIFile(filename: "/usr/lib/llvm-6.0/lib/clang/6.0.0/include/stddef.h", directory: "/home/vtl_server/vtlWorkspace/vtl_server/XTFsPool")
!79 = !DIBasicType(name: "long unsigned int", size: 64, encoding: DW_ATE_unsigned)
!80 = !DIDerivedType(tag: DW_TAG_member, name: "checksum", scope: !67, file: !6, line: 82, baseType: !54, size: 16, offset: 128)
!81 = !DIDerivedType(tag: DW_TAG_member, name: "seq_num", scope: !67, file: !6, line: 83, baseType: !54, size: 16, offset: 144)
!82 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !54, size: 64)
!83 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !84, size: 64)
!84 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "sock_state_t", file: !6, line: 136, size: 64, elements: !85)
!85 = !{!86, !88}
!86 = !DIDerivedType(tag: DW_TAG_member, name: "sk_fd", scope: !84, file: !6, line: 137, baseType: !87, size: 32)
!87 = !DIDerivedType(tag: DW_TAG_typedef, name: "__u32", file: !39, line: 27, baseType: !46)
!88 = !DIDerivedType(tag: DW_TAG_member, name: "event", scope: !84, file: !6, line: 138, baseType: !89, size: 32, offset: 32)
!89 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!90 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !91, size: 64)
!91 = !DIDerivedType(tag: DW_TAG_typedef, name: "vtl_pkt_t", file: !6, line: 94, baseType: !92)
!92 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !6, line: 92, size: 8656, elements: !93)
!93 = !{!94}
!94 = !DIDerivedType(tag: DW_TAG_member, name: "payload", scope: !92, file: !6, line: 93, baseType: !95, size: 8656)
!95 = !DICompositeType(tag: DW_TAG_array_type, baseType: !49, size: 8656, elements: !96)
!96 = !{!97}
!97 = !DISubrange(count: 1082)
!98 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !49, size: 64)
!99 = !{!0, !100, !112, !114, !116, !122, !130, !135, !141, !147, !152, !157}
!100 = !DIGlobalVariableExpression(var: !101, expr: !DIExpression())
!101 = distinct !DIGlobalVariable(name: "NUM_SEQ_MAP", scope: !2, file: !3, line: 30, type: !102, isLocal: false, isDefinition: true)
!102 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "bpf_elf_map", file: !103, line: 18, size: 224, elements: !104)
!103 = !DIFile(filename: "./include/bpf/tc_bpf_util.h", directory: "/home/vtl_server/vtlWorkspace/vtl_server/XTFsPool")
!104 = !{!105, !106, !107, !108, !109, !110, !111}
!105 = !DIDerivedType(tag: DW_TAG_member, name: "type", scope: !102, file: !103, line: 23, baseType: !87, size: 32)
!106 = !DIDerivedType(tag: DW_TAG_member, name: "size_key", scope: !102, file: !103, line: 24, baseType: !87, size: 32, offset: 32)
!107 = !DIDerivedType(tag: DW_TAG_member, name: "size_value", scope: !102, file: !103, line: 25, baseType: !87, size: 32, offset: 64)
!108 = !DIDerivedType(tag: DW_TAG_member, name: "max_elem", scope: !102, file: !103, line: 26, baseType: !87, size: 32, offset: 96)
!109 = !DIDerivedType(tag: DW_TAG_member, name: "flags", scope: !102, file: !103, line: 30, baseType: !87, size: 32, offset: 128)
!110 = !DIDerivedType(tag: DW_TAG_member, name: "id", scope: !102, file: !103, line: 31, baseType: !87, size: 32, offset: 160)
!111 = !DIDerivedType(tag: DW_TAG_member, name: "pinning", scope: !102, file: !103, line: 40, baseType: !87, size: 32, offset: 192)
!112 = !DIGlobalVariableExpression(var: !113, expr: !DIExpression())
!113 = distinct !DIGlobalVariable(name: "BUF_WND_MAP", scope: !2, file: !3, line: 38, type: !102, isLocal: false, isDefinition: true)
!114 = !DIGlobalVariableExpression(var: !115, expr: !DIExpression())
!115 = distinct !DIGlobalVariable(name: "LEN_WND_MAP", scope: !2, file: !3, line: 46, type: !102, isLocal: false, isDefinition: true)
!116 = !DIGlobalVariableExpression(var: !117, expr: !DIExpression())
!117 = distinct !DIGlobalVariable(name: "_license", scope: !2, file: !3, line: 247, type: !118, isLocal: false, isDefinition: true)
!118 = !DICompositeType(tag: DW_TAG_array_type, baseType: !119, size: 32, elements: !120)
!119 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!120 = !{!121}
!121 = !DISubrange(count: 4)
!122 = !DIGlobalVariableExpression(var: !123, expr: !DIExpression())
!123 = distinct !DIGlobalVariable(name: "bpf_trace_printk", scope: !2, file: !124, line: 40, type: !125, isLocal: true, isDefinition: true)
!124 = !DIFile(filename: "./include/bpf/bpf_helpers.h", directory: "/home/vtl_server/vtlWorkspace/vtl_server/XTFsPool")
!125 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !126, size: 64)
!126 = !DISubroutineType(types: !127)
!127 = !{!89, !128, !89, null}
!128 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !129, size: 64)
!129 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !119)
!130 = !DIGlobalVariableExpression(var: !131, expr: !DIExpression())
!131 = distinct !DIGlobalVariable(name: "bpf_map_lookup_elem", scope: !2, file: !124, line: 20, type: !132, isLocal: true, isDefinition: true)
!132 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !133, size: 64)
!133 = !DISubroutineType(types: !134)
!134 = !{!23, !23, !23}
!135 = !DIGlobalVariableExpression(var: !136, expr: !DIExpression())
!136 = distinct !DIGlobalVariable(name: "bpf_map_update_elem", scope: !2, file: !124, line: 22, type: !137, isLocal: true, isDefinition: true)
!137 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !138, size: 64)
!138 = !DISubroutineType(types: !139)
!139 = !{!89, !23, !23, !23, !140}
!140 = !DIBasicType(name: "long long unsigned int", size: 64, encoding: DW_ATE_unsigned)
!141 = !DIGlobalVariableExpression(var: !142, expr: !DIExpression())
!142 = distinct !DIGlobalVariable(name: "bpf_skb_change_tail", scope: !2, file: !124, line: 266, type: !143, isLocal: true, isDefinition: true)
!143 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !144, size: 64)
!144 = !DISubroutineType(types: !145)
!145 = !{!89, !23, !87, !146}
!146 = !DIDerivedType(tag: DW_TAG_typedef, name: "__u64", file: !39, line: 31, baseType: !140)
!147 = !DIGlobalVariableExpression(var: !148, expr: !DIExpression())
!148 = distinct !DIGlobalVariable(name: "bpf_clone_redirect", scope: !2, file: !124, line: 57, type: !149, isLocal: true, isDefinition: true)
!149 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !150, size: 64)
!150 = !DISubroutineType(types: !151)
!151 = !{!89, !23, !89, !89}
!152 = !DIGlobalVariableExpression(var: !153, expr: !DIExpression())
!153 = distinct !DIGlobalVariable(name: "bpf_vtl_start_timer", scope: !2, file: !124, line: 38, type: !154, isLocal: true, isDefinition: true)
!154 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !155, size: 64)
!155 = !DISubroutineType(types: !156)
!156 = !{null, !140}
!157 = !DIGlobalVariableExpression(var: !158, expr: !DIExpression())
!158 = distinct !DIGlobalVariable(name: "bpf_vtl_store_bytes", scope: !2, file: !124, line: 240, type: !159, isLocal: true, isDefinition: true)
!159 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !160, size: 64)
!160 = !DISubroutineType(types: !161)
!161 = !{!89, !23, !89, !23, !89, !89}
!162 = !{i32 2, !"Dwarf Version", i32 4}
!163 = !{i32 2, !"Debug Info Version", i32 3}
!164 = !{i32 1, !"wchar_size", i32 4}
!165 = !{!"clang version 6.0.0-1ubuntu2 (tags/RELEASE_600/final)"}
!166 = distinct !DISubprogram(name: "_tf_tc_egress", scope: !3, file: !3, line: 55, type: !167, isLocal: false, isDefinition: true, scopeLine: 55, flags: DIFlagPrototyped, isOptimized: true, unit: !2, variables: !257)
!167 = !DISubroutineType(types: !168)
!168 = !{!89, !169}
!169 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !170, size: 64)
!170 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "__sk_buff", file: !15, line: 2680, size: 1408, elements: !171)
!171 = !{!172, !173, !174, !175, !176, !177, !178, !179, !180, !181, !182, !183, !184, !188, !189, !190, !191, !192, !193, !194, !195, !196, !198, !199, !200, !201, !202, !234, !235, !236, !237}
!172 = !DIDerivedType(tag: DW_TAG_member, name: "len", scope: !170, file: !15, line: 2681, baseType: !87, size: 32)
!173 = !DIDerivedType(tag: DW_TAG_member, name: "pkt_type", scope: !170, file: !15, line: 2682, baseType: !87, size: 32, offset: 32)
!174 = !DIDerivedType(tag: DW_TAG_member, name: "mark", scope: !170, file: !15, line: 2683, baseType: !87, size: 32, offset: 64)
!175 = !DIDerivedType(tag: DW_TAG_member, name: "queue_mapping", scope: !170, file: !15, line: 2684, baseType: !87, size: 32, offset: 96)
!176 = !DIDerivedType(tag: DW_TAG_member, name: "protocol", scope: !170, file: !15, line: 2685, baseType: !87, size: 32, offset: 128)
!177 = !DIDerivedType(tag: DW_TAG_member, name: "vlan_present", scope: !170, file: !15, line: 2686, baseType: !87, size: 32, offset: 160)
!178 = !DIDerivedType(tag: DW_TAG_member, name: "vlan_tci", scope: !170, file: !15, line: 2687, baseType: !87, size: 32, offset: 192)
!179 = !DIDerivedType(tag: DW_TAG_member, name: "vlan_proto", scope: !170, file: !15, line: 2688, baseType: !87, size: 32, offset: 224)
!180 = !DIDerivedType(tag: DW_TAG_member, name: "priority", scope: !170, file: !15, line: 2689, baseType: !87, size: 32, offset: 256)
!181 = !DIDerivedType(tag: DW_TAG_member, name: "ingress_ifindex", scope: !170, file: !15, line: 2690, baseType: !87, size: 32, offset: 288)
!182 = !DIDerivedType(tag: DW_TAG_member, name: "ifindex", scope: !170, file: !15, line: 2691, baseType: !87, size: 32, offset: 320)
!183 = !DIDerivedType(tag: DW_TAG_member, name: "tc_index", scope: !170, file: !15, line: 2692, baseType: !87, size: 32, offset: 352)
!184 = !DIDerivedType(tag: DW_TAG_member, name: "cb", scope: !170, file: !15, line: 2693, baseType: !185, size: 160, offset: 384)
!185 = !DICompositeType(tag: DW_TAG_array_type, baseType: !87, size: 160, elements: !186)
!186 = !{!187}
!187 = !DISubrange(count: 5)
!188 = !DIDerivedType(tag: DW_TAG_member, name: "hash", scope: !170, file: !15, line: 2694, baseType: !87, size: 32, offset: 544)
!189 = !DIDerivedType(tag: DW_TAG_member, name: "tc_classid", scope: !170, file: !15, line: 2695, baseType: !87, size: 32, offset: 576)
!190 = !DIDerivedType(tag: DW_TAG_member, name: "data", scope: !170, file: !15, line: 2696, baseType: !87, size: 32, offset: 608)
!191 = !DIDerivedType(tag: DW_TAG_member, name: "data_end", scope: !170, file: !15, line: 2697, baseType: !87, size: 32, offset: 640)
!192 = !DIDerivedType(tag: DW_TAG_member, name: "napi_id", scope: !170, file: !15, line: 2698, baseType: !87, size: 32, offset: 672)
!193 = !DIDerivedType(tag: DW_TAG_member, name: "family", scope: !170, file: !15, line: 2701, baseType: !87, size: 32, offset: 704)
!194 = !DIDerivedType(tag: DW_TAG_member, name: "remote_ip4", scope: !170, file: !15, line: 2702, baseType: !87, size: 32, offset: 736)
!195 = !DIDerivedType(tag: DW_TAG_member, name: "local_ip4", scope: !170, file: !15, line: 2703, baseType: !87, size: 32, offset: 768)
!196 = !DIDerivedType(tag: DW_TAG_member, name: "remote_ip6", scope: !170, file: !15, line: 2704, baseType: !197, size: 128, offset: 800)
!197 = !DICompositeType(tag: DW_TAG_array_type, baseType: !87, size: 128, elements: !120)
!198 = !DIDerivedType(tag: DW_TAG_member, name: "local_ip6", scope: !170, file: !15, line: 2705, baseType: !197, size: 128, offset: 928)
!199 = !DIDerivedType(tag: DW_TAG_member, name: "remote_port", scope: !170, file: !15, line: 2706, baseType: !87, size: 32, offset: 1056)
!200 = !DIDerivedType(tag: DW_TAG_member, name: "local_port", scope: !170, file: !15, line: 2707, baseType: !87, size: 32, offset: 1088)
!201 = !DIDerivedType(tag: DW_TAG_member, name: "data_meta", scope: !170, file: !15, line: 2710, baseType: !87, size: 32, offset: 1120)
!202 = !DIDerivedType(tag: DW_TAG_member, scope: !170, file: !15, line: 2711, baseType: !203, size: 64, align: 64, offset: 1152)
!203 = distinct !DICompositeType(tag: DW_TAG_union_type, scope: !170, file: !15, line: 2711, size: 64, align: 64, elements: !204)
!204 = !{!205}
!205 = !DIDerivedType(tag: DW_TAG_member, name: "flow_keys", scope: !203, file: !15, line: 2711, baseType: !206, size: 64)
!206 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !207, size: 64)
!207 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "bpf_flow_keys", file: !15, line: 3243, size: 384, elements: !208)
!208 = !{!209, !210, !211, !212, !214, !215, !216, !217, !218, !219, !220}
!209 = !DIDerivedType(tag: DW_TAG_member, name: "nhoff", scope: !207, file: !15, line: 3244, baseType: !38, size: 16)
!210 = !DIDerivedType(tag: DW_TAG_member, name: "thoff", scope: !207, file: !15, line: 3245, baseType: !38, size: 16, offset: 16)
!211 = !DIDerivedType(tag: DW_TAG_member, name: "addr_proto", scope: !207, file: !15, line: 3246, baseType: !38, size: 16, offset: 32)
!212 = !DIDerivedType(tag: DW_TAG_member, name: "is_frag", scope: !207, file: !15, line: 3247, baseType: !213, size: 8, offset: 48)
!213 = !DIDerivedType(tag: DW_TAG_typedef, name: "__u8", file: !39, line: 21, baseType: !31)
!214 = !DIDerivedType(tag: DW_TAG_member, name: "is_first_frag", scope: !207, file: !15, line: 3248, baseType: !213, size: 8, offset: 56)
!215 = !DIDerivedType(tag: DW_TAG_member, name: "is_encap", scope: !207, file: !15, line: 3249, baseType: !213, size: 8, offset: 64)
!216 = !DIDerivedType(tag: DW_TAG_member, name: "ip_proto", scope: !207, file: !15, line: 3250, baseType: !213, size: 8, offset: 72)
!217 = !DIDerivedType(tag: DW_TAG_member, name: "n_proto", scope: !207, file: !15, line: 3251, baseType: !36, size: 16, offset: 80)
!218 = !DIDerivedType(tag: DW_TAG_member, name: "sport", scope: !207, file: !15, line: 3252, baseType: !36, size: 16, offset: 96)
!219 = !DIDerivedType(tag: DW_TAG_member, name: "dport", scope: !207, file: !15, line: 3253, baseType: !36, size: 16, offset: 112)
!220 = !DIDerivedType(tag: DW_TAG_member, scope: !207, file: !15, line: 3254, baseType: !221, size: 256, offset: 128)
!221 = distinct !DICompositeType(tag: DW_TAG_union_type, scope: !207, file: !15, line: 3254, size: 256, elements: !222)
!222 = !{!223, !229}
!223 = !DIDerivedType(tag: DW_TAG_member, scope: !221, file: !15, line: 3255, baseType: !224, size: 64)
!224 = distinct !DICompositeType(tag: DW_TAG_structure_type, scope: !221, file: !15, line: 3255, size: 64, elements: !225)
!225 = !{!226, !228}
!226 = !DIDerivedType(tag: DW_TAG_member, name: "ipv4_src", scope: !224, file: !15, line: 3256, baseType: !227, size: 32)
!227 = !DIDerivedType(tag: DW_TAG_typedef, name: "__be32", file: !37, line: 27, baseType: !87)
!228 = !DIDerivedType(tag: DW_TAG_member, name: "ipv4_dst", scope: !224, file: !15, line: 3257, baseType: !227, size: 32, offset: 32)
!229 = !DIDerivedType(tag: DW_TAG_member, scope: !221, file: !15, line: 3259, baseType: !230, size: 256)
!230 = distinct !DICompositeType(tag: DW_TAG_structure_type, scope: !221, file: !15, line: 3259, size: 256, elements: !231)
!231 = !{!232, !233}
!232 = !DIDerivedType(tag: DW_TAG_member, name: "ipv6_src", scope: !230, file: !15, line: 3260, baseType: !197, size: 128)
!233 = !DIDerivedType(tag: DW_TAG_member, name: "ipv6_dst", scope: !230, file: !15, line: 3261, baseType: !197, size: 128, offset: 128)
!234 = !DIDerivedType(tag: DW_TAG_member, name: "tstamp", scope: !170, file: !15, line: 2712, baseType: !146, size: 64, offset: 1216)
!235 = !DIDerivedType(tag: DW_TAG_member, name: "wire_len", scope: !170, file: !15, line: 2713, baseType: !87, size: 32, offset: 1280)
!236 = !DIDerivedType(tag: DW_TAG_member, name: "gso_segs", scope: !170, file: !15, line: 2714, baseType: !87, size: 32, offset: 1312)
!237 = !DIDerivedType(tag: DW_TAG_member, scope: !170, file: !15, line: 2715, baseType: !238, size: 64, align: 64, offset: 1344)
!238 = distinct !DICompositeType(tag: DW_TAG_union_type, scope: !170, file: !15, line: 2715, size: 64, align: 64, elements: !239)
!239 = !{!240}
!240 = !DIDerivedType(tag: DW_TAG_member, name: "sk", scope: !238, file: !15, line: 2715, baseType: !241, size: 64)
!241 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !242, size: 64)
!242 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "bpf_sock", file: !15, line: 2768, size: 608, elements: !243)
!243 = !{!244, !245, !246, !247, !248, !249, !250, !251, !252, !253, !254, !255, !256}
!244 = !DIDerivedType(tag: DW_TAG_member, name: "bound_dev_if", scope: !242, file: !15, line: 2769, baseType: !87, size: 32)
!245 = !DIDerivedType(tag: DW_TAG_member, name: "family", scope: !242, file: !15, line: 2770, baseType: !87, size: 32, offset: 32)
!246 = !DIDerivedType(tag: DW_TAG_member, name: "type", scope: !242, file: !15, line: 2771, baseType: !87, size: 32, offset: 64)
!247 = !DIDerivedType(tag: DW_TAG_member, name: "protocol", scope: !242, file: !15, line: 2772, baseType: !87, size: 32, offset: 96)
!248 = !DIDerivedType(tag: DW_TAG_member, name: "mark", scope: !242, file: !15, line: 2773, baseType: !87, size: 32, offset: 128)
!249 = !DIDerivedType(tag: DW_TAG_member, name: "priority", scope: !242, file: !15, line: 2774, baseType: !87, size: 32, offset: 160)
!250 = !DIDerivedType(tag: DW_TAG_member, name: "src_ip4", scope: !242, file: !15, line: 2776, baseType: !87, size: 32, offset: 192)
!251 = !DIDerivedType(tag: DW_TAG_member, name: "src_ip6", scope: !242, file: !15, line: 2777, baseType: !197, size: 128, offset: 224)
!252 = !DIDerivedType(tag: DW_TAG_member, name: "src_port", scope: !242, file: !15, line: 2778, baseType: !87, size: 32, offset: 352)
!253 = !DIDerivedType(tag: DW_TAG_member, name: "dst_port", scope: !242, file: !15, line: 2779, baseType: !87, size: 32, offset: 384)
!254 = !DIDerivedType(tag: DW_TAG_member, name: "dst_ip4", scope: !242, file: !15, line: 2780, baseType: !87, size: 32, offset: 416)
!255 = !DIDerivedType(tag: DW_TAG_member, name: "dst_ip6", scope: !242, file: !15, line: 2781, baseType: !197, size: 128, offset: 448)
!256 = !DIDerivedType(tag: DW_TAG_member, name: "state", scope: !242, file: !15, line: 2782, baseType: !87, size: 32, offset: 576)
!257 = !{!258, !259, !260, !261, !262, !263, !264, !265, !266, !267, !268, !269, !271, !272, !273, !274, !275, !276, !279, !280, !281, !282, !289}
!258 = !DILocalVariable(name: "skb", arg: 1, scope: !166, file: !3, line: 55, type: !169)
!259 = !DILocalVariable(name: "data", scope: !166, file: !3, line: 57, type: !23)
!260 = !DILocalVariable(name: "data_end", scope: !166, file: !3, line: 58, type: !23)
!261 = !DILocalVariable(name: "eth", scope: !166, file: !3, line: 60, type: !25)
!262 = !DILocalVariable(name: "iph", scope: !166, file: !3, line: 64, type: !41)
!263 = !DILocalVariable(name: "vtlh", scope: !166, file: !3, line: 71, type: !65)
!264 = !DILocalVariable(name: "sk_state", scope: !166, file: !3, line: 75, type: !83)
!265 = !DILocalVariable(name: "save_skb", scope: !166, file: !3, line: 76, type: !90)
!266 = !DILocalVariable(name: "save_len", scope: !166, file: !3, line: 77, type: !54)
!267 = !DILocalVariable(name: "id_seq", scope: !166, file: !3, line: 79, type: !89)
!268 = !DILocalVariable(name: "id_ack_buf", scope: !166, file: !3, line: 79, type: !89)
!269 = !DILocalVariable(name: "last_len", scope: !166, file: !3, line: 80, type: !270)
!270 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !54)
!271 = !DILocalVariable(name: "num_seq", scope: !166, file: !3, line: 81, type: !82)
!272 = !DILocalVariable(name: "sum", scope: !166, file: !3, line: 87, type: !54)
!273 = !DILocalVariable(name: "cur_pload_len", scope: !166, file: !3, line: 106, type: !62)
!274 = !DILocalVariable(name: "cur_win_size", scope: !166, file: !3, line: 133, type: !270)
!275 = !DILocalVariable(name: "tx_num", scope: !166, file: !3, line: 140, type: !89)
!276 = !DILocalVariable(name: "cursor", scope: !277, file: !3, line: 142, type: !46)
!277 = distinct !DILexicalBlock(scope: !278, file: !3, line: 141, column: 62)
!278 = distinct !DILexicalBlock(scope: !166, file: !3, line: 141, column: 7)
!279 = !DILocalVariable(name: "i", scope: !277, file: !3, line: 142, type: !46)
!280 = !DILocalVariable(name: "j", scope: !277, file: !3, line: 142, type: !46)
!281 = !DILocalVariable(name: "lost", scope: !277, file: !3, line: 142, type: !46)
!282 = !DILocalVariable(name: "get_skb", scope: !283, file: !3, line: 177, type: !90)
!283 = distinct !DILexicalBlock(scope: !284, file: !3, line: 170, column: 48)
!284 = distinct !DILexicalBlock(scope: !285, file: !3, line: 170, column: 13)
!285 = distinct !DILexicalBlock(scope: !286, file: !3, line: 170, column: 13)
!286 = distinct !DILexicalBlock(scope: !287, file: !3, line: 167, column: 20)
!287 = distinct !DILexicalBlock(scope: !288, file: !3, line: 167, column: 14)
!288 = distinct !DILexicalBlock(scope: !277, file: !3, line: 143, column: 11)
!289 = !DILocalVariable(name: "get_len", scope: !283, file: !3, line: 181, type: !82)
!290 = !DILocalVariable(name: "____fmt", scope: !291, file: !6, line: 228, type: !317)
!291 = distinct !DILexicalBlock(scope: !292, file: !6, line: 228, column: 7)
!292 = distinct !DILexicalBlock(scope: !293, file: !6, line: 227, column: 27)
!293 = distinct !DILexicalBlock(scope: !294, file: !6, line: 227, column: 7)
!294 = distinct !DISubprogram(name: "vtl_csum", scope: !6, file: !6, line: 221, type: !295, isLocal: true, isDefinition: true, scopeLine: 221, flags: DIFlagPrototyped, isOptimized: true, unit: !2, variables: !297)
!295 = !DISubroutineType(types: !296)
!296 = !{!89, !23, !23, !82}
!297 = !{!298, !299, !300, !301, !302, !303, !290, !304, !305, !312, !313, !320, !321, !328}
!298 = !DILocalVariable(name: "data", arg: 1, scope: !294, file: !6, line: 221, type: !23)
!299 = !DILocalVariable(name: "data_end", arg: 2, scope: !294, file: !6, line: 221, type: !23)
!300 = !DILocalVariable(name: "csum", arg: 3, scope: !294, file: !6, line: 221, type: !82)
!301 = !DILocalVariable(name: "y", scope: !294, file: !6, line: 223, type: !89)
!302 = !DILocalVariable(name: "sum", scope: !294, file: !6, line: 224, type: !49)
!303 = !DILocalVariable(name: "eth", scope: !294, file: !6, line: 226, type: !25)
!304 = !DILocalVariable(name: "iph", scope: !294, file: !6, line: 232, type: !41)
!305 = !DILocalVariable(name: "____fmt", scope: !306, file: !6, line: 234, type: !309)
!306 = distinct !DILexicalBlock(scope: !307, file: !6, line: 234, column: 7)
!307 = distinct !DILexicalBlock(scope: !308, file: !6, line: 233, column: 27)
!308 = distinct !DILexicalBlock(scope: !294, file: !6, line: 233, column: 7)
!309 = !DICompositeType(tag: DW_TAG_array_type, baseType: !119, size: 272, elements: !310)
!310 = !{!311}
!311 = !DISubrange(count: 34)
!312 = !DILocalVariable(name: "vtlh", scope: !294, file: !6, line: 242, type: !65)
!313 = !DILocalVariable(name: "____fmt", scope: !314, file: !6, line: 244, type: !317)
!314 = distinct !DILexicalBlock(scope: !315, file: !6, line: 244, column: 7)
!315 = distinct !DILexicalBlock(scope: !316, file: !6, line: 243, column: 28)
!316 = distinct !DILexicalBlock(scope: !294, file: !6, line: 243, column: 7)
!317 = !DICompositeType(tag: DW_TAG_array_type, baseType: !119, size: 280, elements: !318)
!318 = !{!319}
!319 = !DISubrange(count: 35)
!320 = !DILocalVariable(name: "d", scope: !294, file: !6, line: 248, type: !98)
!321 = !DILocalVariable(name: "____fmt", scope: !322, file: !6, line: 250, type: !325)
!322 = distinct !DILexicalBlock(scope: !323, file: !6, line: 250, column: 7)
!323 = distinct !DILexicalBlock(scope: !324, file: !6, line: 249, column: 25)
!324 = distinct !DILexicalBlock(scope: !294, file: !6, line: 249, column: 7)
!325 = !DICompositeType(tag: DW_TAG_array_type, baseType: !119, size: 248, elements: !326)
!326 = !{!327}
!327 = !DISubrange(count: 31)
!328 = !DILocalVariable(name: "block", scope: !329, file: !6, line: 259, type: !98)
!329 = distinct !DILexicalBlock(scope: !330, file: !6, line: 254, column: 43)
!330 = distinct !DILexicalBlock(scope: !331, file: !6, line: 254, column: 4)
!331 = distinct !DILexicalBlock(scope: !294, file: !6, line: 254, column: 4)
!332 = !DILocation(line: 228, column: 7, scope: !291, inlinedAt: !333)
!333 = distinct !DILocation(line: 88, column: 7, scope: !334)
!334 = distinct !DILexicalBlock(scope: !166, file: !3, line: 88, column: 7)
!335 = !DILocation(line: 234, column: 7, scope: !306, inlinedAt: !333)
!336 = !DILocation(line: 244, column: 7, scope: !314, inlinedAt: !333)
!337 = !DILocation(line: 250, column: 7, scope: !322, inlinedAt: !333)
!338 = !DILocation(line: 55, column: 37, scope: !166)
!339 = !DILocation(line: 57, column: 36, scope: !166)
!340 = !{!341, !342, i64 76}
!341 = !{!"__sk_buff", !342, i64 0, !342, i64 4, !342, i64 8, !342, i64 12, !342, i64 16, !342, i64 20, !342, i64 24, !342, i64 28, !342, i64 32, !342, i64 36, !342, i64 40, !342, i64 44, !343, i64 48, !342, i64 68, !342, i64 72, !342, i64 76, !342, i64 80, !342, i64 84, !342, i64 88, !342, i64 92, !342, i64 96, !343, i64 100, !343, i64 116, !342, i64 132, !342, i64 136, !342, i64 140, !343, i64 144, !345, i64 152, !342, i64 160, !342, i64 164, !343, i64 168}
!342 = !{!"int", !343, i64 0}
!343 = !{!"omnipotent char", !344, i64 0}
!344 = !{!"Simple C/C++ TBAA"}
!345 = !{!"long long", !343, i64 0}
!346 = !DILocation(line: 57, column: 25, scope: !166)
!347 = !DILocation(line: 57, column: 17, scope: !166)
!348 = !DILocation(line: 57, column: 10, scope: !166)
!349 = !DILocation(line: 58, column: 40, scope: !166)
!350 = !{!341, !342, i64 80}
!351 = !DILocation(line: 58, column: 29, scope: !166)
!352 = !DILocation(line: 58, column: 21, scope: !166)
!353 = !DILocation(line: 58, column: 10, scope: !166)
!354 = !DILocation(line: 60, column: 25, scope: !166)
!355 = !DILocation(line: 60, column: 19, scope: !166)
!356 = !DILocation(line: 61, column: 11, scope: !357)
!357 = distinct !DILexicalBlock(scope: !166, file: !3, line: 61, column: 7)
!358 = !DILocation(line: 61, column: 17, scope: !357)
!359 = !DILocation(line: 61, column: 15, scope: !357)
!360 = !DILocation(line: 61, column: 7, scope: !166)
!361 = !DILocation(line: 64, column: 18, scope: !166)
!362 = !DILocation(line: 65, column: 11, scope: !363)
!363 = distinct !DILexicalBlock(scope: !166, file: !3, line: 65, column: 7)
!364 = !DILocation(line: 65, column: 17, scope: !363)
!365 = !DILocation(line: 65, column: 15, scope: !363)
!366 = !DILocation(line: 65, column: 7, scope: !166)
!367 = !DILocation(line: 68, column: 12, scope: !368)
!368 = distinct !DILexicalBlock(scope: !166, file: !3, line: 68, column: 7)
!369 = !{!370, !343, i64 9}
!370 = !{!"iphdr", !342, i64 0, !342, i64 0, !343, i64 1, !371, i64 2, !371, i64 4, !371, i64 6, !343, i64 8, !343, i64 9, !371, i64 10, !342, i64 12, !342, i64 16}
!371 = !{!"short", !343, i64 0}
!372 = !DILocation(line: 68, column: 21, scope: !368)
!373 = !DILocation(line: 68, column: 7, scope: !166)
!374 = !DILocation(line: 71, column: 15, scope: !166)
!375 = !DILocation(line: 72, column: 12, scope: !376)
!376 = distinct !DILexicalBlock(scope: !166, file: !3, line: 72, column: 7)
!377 = !DILocation(line: 72, column: 18, scope: !376)
!378 = !DILocation(line: 72, column: 16, scope: !376)
!379 = !DILocation(line: 72, column: 7, scope: !166)
!380 = !DILocation(line: 75, column: 25, scope: !166)
!381 = !DILocation(line: 76, column: 15, scope: !166)
!382 = !DILocation(line: 77, column: 9, scope: !166)
!383 = !DILocation(line: 77, column: 18, scope: !166)
!384 = !{!371, !371, i64 0}
!385 = !DILocation(line: 79, column: 4, scope: !166)
!386 = !DILocation(line: 79, column: 8, scope: !166)
!387 = !{!342, !342, i64 0}
!388 = !DILocation(line: 79, column: 20, scope: !166)
!389 = !DILocation(line: 80, column: 51, scope: !166)
!390 = !{!341, !342, i64 0}
!391 = !DILocation(line: 81, column: 14, scope: !166)
!392 = !DILocation(line: 85, column: 10, scope: !166)
!393 = !DILocation(line: 85, column: 19, scope: !166)
!394 = !{!395, !343, i64 4}
!395 = !{!"", !343, i64 0, !343, i64 4, !396, i64 8, !371, i64 16, !371, i64 18}
!396 = !{!"long", !343, i64 0}
!397 = !DILocation(line: 87, column: 13, scope: !166)
!398 = !DILocation(line: 221, column: 43, scope: !294, inlinedAt: !333)
!399 = !DILocation(line: 221, column: 55, scope: !294, inlinedAt: !333)
!400 = !DILocation(line: 223, column: 6, scope: !294, inlinedAt: !333)
!401 = !DILocation(line: 224, column: 10, scope: !294, inlinedAt: !333)
!402 = !DILocation(line: 226, column: 17, scope: !294, inlinedAt: !333)
!403 = !DILocation(line: 227, column: 11, scope: !293, inlinedAt: !333)
!404 = !DILocation(line: 227, column: 15, scope: !293, inlinedAt: !333)
!405 = !DILocation(line: 227, column: 7, scope: !294, inlinedAt: !333)
!406 = !DILocation(line: 228, column: 7, scope: !292, inlinedAt: !333)
!407 = !DILocation(line: 229, column: 7, scope: !292, inlinedAt: !333)
!408 = !DILocation(line: 232, column: 18, scope: !294, inlinedAt: !333)
!409 = !DILocation(line: 233, column: 11, scope: !308, inlinedAt: !333)
!410 = !DILocation(line: 233, column: 15, scope: !308, inlinedAt: !333)
!411 = !DILocation(line: 233, column: 7, scope: !294, inlinedAt: !333)
!412 = !DILocation(line: 234, column: 7, scope: !307, inlinedAt: !333)
!413 = !DILocation(line: 235, column: 7, scope: !307, inlinedAt: !333)
!414 = !DILocation(line: 238, column: 12, scope: !415, inlinedAt: !333)
!415 = distinct !DILexicalBlock(scope: !294, file: !6, line: 238, column: 7)
!416 = !DILocation(line: 238, column: 21, scope: !415, inlinedAt: !333)
!417 = !DILocation(line: 238, column: 7, scope: !294, inlinedAt: !333)
!418 = !DILocation(line: 242, column: 15, scope: !294, inlinedAt: !333)
!419 = !DILocation(line: 243, column: 12, scope: !316, inlinedAt: !333)
!420 = !DILocation(line: 243, column: 16, scope: !316, inlinedAt: !333)
!421 = !DILocation(line: 243, column: 7, scope: !294, inlinedAt: !333)
!422 = !DILocation(line: 244, column: 7, scope: !315, inlinedAt: !333)
!423 = !DILocation(line: 245, column: 7, scope: !315, inlinedAt: !333)
!424 = !DILocation(line: 248, column: 13, scope: !294, inlinedAt: !333)
!425 = !DILocation(line: 249, column: 9, scope: !324, inlinedAt: !333)
!426 = !DILocation(line: 249, column: 13, scope: !324, inlinedAt: !333)
!427 = !DILocation(line: 249, column: 7, scope: !294, inlinedAt: !333)
!428 = !DILocation(line: 250, column: 7, scope: !323, inlinedAt: !333)
!429 = !DILocation(line: 251, column: 7, scope: !323, inlinedAt: !333)
!430 = !DILocation(line: 254, column: 25, scope: !330, inlinedAt: !333)
!431 = !{!395, !396, i64 8}
!432 = !DILocation(line: 254, column: 17, scope: !330, inlinedAt: !333)
!433 = !DILocation(line: 254, column: 4, scope: !331, inlinedAt: !333)
!434 = !DILocation(line: 259, column: 38, scope: !329, inlinedAt: !333)
!435 = !DILocation(line: 259, column: 42, scope: !329, inlinedAt: !333)
!436 = !DILocation(line: 259, column: 16, scope: !329, inlinedAt: !333)
!437 = !DILocation(line: 260, column: 16, scope: !438, inlinedAt: !333)
!438 = distinct !DILexicalBlock(scope: !329, file: !6, line: 260, column: 10)
!439 = !DILocation(line: 260, column: 20, scope: !438, inlinedAt: !333)
!440 = !DILocation(line: 260, column: 10, scope: !329, inlinedAt: !333)
!441 = !DILocation(line: 263, column: 14, scope: !329, inlinedAt: !333)
!442 = !{!343, !343, i64 0}
!443 = !DILocation(line: 263, column: 11, scope: !329, inlinedAt: !333)
!444 = !DILocation(line: 254, column: 39, scope: !330, inlinedAt: !333)
!445 = !DILocation(line: 256, column: 10, scope: !446, inlinedAt: !333)
!446 = distinct !DILexicalBlock(scope: !329, file: !6, line: 256, column: 8)
!447 = distinct !{!447, !448, !449}
!448 = !DILocation(line: 254, column: 4, scope: !331)
!449 = !DILocation(line: 264, column: 4, scope: !331)
!450 = !DILocation(line: 265, column: 12, scope: !294, inlinedAt: !333)
!451 = !DILocation(line: 90, column: 10, scope: !166)
!452 = !DILocation(line: 90, column: 19, scope: !166)
!453 = !{!395, !371, i64 16}
!454 = !DILocation(line: 92, column: 27, scope: !166)
!455 = !DILocation(line: 92, column: 14, scope: !166)
!456 = !DILocation(line: 93, column: 8, scope: !457)
!457 = distinct !DILexicalBlock(scope: !166, file: !3, line: 93, column: 7)
!458 = !DILocation(line: 93, column: 7, scope: !166)
!459 = !DILocation(line: 95, column: 20, scope: !166)
!460 = !DILocation(line: 95, column: 10, scope: !166)
!461 = !DILocation(line: 95, column: 18, scope: !166)
!462 = !{!395, !371, i64 18}
!463 = !DILocation(line: 99, column: 32, scope: !166)
!464 = !DILocation(line: 99, column: 17, scope: !166)
!465 = !DILocation(line: 99, column: 15, scope: !166)
!466 = !DILocation(line: 100, column: 39, scope: !166)
!467 = !DILocation(line: 101, column: 8, scope: !468)
!468 = distinct !DILexicalBlock(scope: !166, file: !3, line: 101, column: 7)
!469 = !DILocation(line: 101, column: 7, scope: !166)
!470 = !DILocation(line: 103, column: 14, scope: !166)
!471 = !DILocation(line: 103, column: 20, scope: !166)
!472 = !{!473, !342, i64 4}
!473 = !{!"sock_state_t", !342, i64 0, !342, i64 4}
!474 = !DILocation(line: 104, column: 4, scope: !166)
!475 = !DILocation(line: 106, column: 46, scope: !166)
!476 = !DILocation(line: 106, column: 29, scope: !166)
!477 = !DILocation(line: 106, column: 13, scope: !166)
!478 = !DILocation(line: 107, column: 21, scope: !479)
!479 = distinct !DILexicalBlock(scope: !166, file: !3, line: 107, column: 7)
!480 = !DILocation(line: 107, column: 7, scope: !166)
!481 = !DILocation(line: 108, column: 33, scope: !482)
!482 = distinct !DILexicalBlock(scope: !479, file: !3, line: 107, column: 40)
!483 = !DILocation(line: 108, column: 28, scope: !482)
!484 = !DILocation(line: 108, column: 26, scope: !482)
!485 = !DILocation(line: 109, column: 17, scope: !482)
!486 = !DILocation(line: 111, column: 27, scope: !482)
!487 = !DILocation(line: 111, column: 7, scope: !482)
!488 = !DILocation(line: 113, column: 29, scope: !482)
!489 = !DILocation(line: 113, column: 18, scope: !482)
!490 = !DILocation(line: 114, column: 33, scope: !482)
!491 = !DILocation(line: 114, column: 22, scope: !482)
!492 = !DILocation(line: 115, column: 21, scope: !482)
!493 = !DILocation(line: 116, column: 15, scope: !494)
!494 = distinct !DILexicalBlock(scope: !482, file: !3, line: 116, column: 6)
!495 = !DILocation(line: 116, column: 21, scope: !494)
!496 = !DILocation(line: 116, column: 19, scope: !494)
!497 = !DILocation(line: 116, column: 6, scope: !482)
!498 = !DILocation(line: 113, column: 10, scope: !482)
!499 = !DILocation(line: 118, column: 3, scope: !482)
!500 = !DILocation(line: 120, column: 28, scope: !482)
!501 = !DILocation(line: 120, column: 3, scope: !482)
!502 = !DILocation(line: 121, column: 2, scope: !482)
!503 = !DILocation(line: 123, column: 26, scope: !504)
!504 = distinct !DILexicalBlock(scope: !479, file: !3, line: 122, column: 7)
!505 = !DILocation(line: 124, column: 17, scope: !504)
!506 = !DILocation(line: 126, column: 14, scope: !504)
!507 = !DILocation(line: 127, column: 15, scope: !508)
!508 = distinct !DILexicalBlock(scope: !504, file: !3, line: 127, column: 6)
!509 = !DILocation(line: 127, column: 21, scope: !508)
!510 = !DILocation(line: 127, column: 19, scope: !508)
!511 = !DILocation(line: 127, column: 6, scope: !504)
!512 = !DILocation(line: 129, column: 3, scope: !504)
!513 = !DILocation(line: 138, column: 23, scope: !166)
!514 = !DILocation(line: 133, column: 56, scope: !166)
!515 = !DILocation(line: 133, column: 24, scope: !166)
!516 = !DILocation(line: 134, column: 14, scope: !166)
!517 = !DILocation(line: 135, column: 13, scope: !166)
!518 = !DILocation(line: 136, column: 4, scope: !166)
!519 = !DILocation(line: 138, column: 33, scope: !166)
!520 = !{!341, !342, i64 40}
!521 = !DILocation(line: 138, column: 40, scope: !166)
!522 = !DILocation(line: 138, column: 4, scope: !166)
!523 = !DILocation(line: 140, column: 8, scope: !166)
!524 = !DILocation(line: 141, column: 7, scope: !278)
!525 = !DILocation(line: 141, column: 18, scope: !278)
!526 = !DILocation(line: 141, column: 37, scope: !278)
!527 = !DILocation(line: 141, column: 46, scope: !278)
!528 = !DILocation(line: 141, column: 34, scope: !278)
!529 = !DILocation(line: 142, column: 21, scope: !277)
!530 = !DILocation(line: 142, column: 33, scope: !277)
!531 = !DILocation(line: 142, column: 40, scope: !277)
!532 = !DILocation(line: 142, column: 47, scope: !277)
!533 = !DILocation(line: 144, column: 11, scope: !288)
!534 = !DILocation(line: 147, column: 14, scope: !288)
!535 = !DILocation(line: 150, column: 29, scope: !536)
!536 = distinct !DILexicalBlock(scope: !537, file: !3, line: 150, column: 11)
!537 = distinct !DILexicalBlock(scope: !288, file: !3, line: 150, column: 11)
!538 = !DILocation(line: 152, column: 38, scope: !539)
!539 = distinct !DILexicalBlock(scope: !540, file: !3, line: 152, column: 36)
!540 = distinct !DILexicalBlock(scope: !536, file: !3, line: 150, column: 46)
!541 = !DILocation(line: 150, column: 11, scope: !537)
!542 = !DILocation(line: 155, column: 44, scope: !540)
!543 = !DILocation(line: 157, column: 48, scope: !540)
!544 = !DILocation(line: 158, column: 17, scope: !545)
!545 = distinct !DILexicalBlock(scope: !540, file: !3, line: 158, column: 16)
!546 = !DILocation(line: 158, column: 16, scope: !540)
!547 = !DILocation(line: 161, column: 26, scope: !548)
!548 = distinct !DILexicalBlock(scope: !540, file: !3, line: 161, column: 16)
!549 = !DILocation(line: 161, column: 32, scope: !548)
!550 = !DILocation(line: 161, column: 16, scope: !540)
!551 = !DILocation(line: 150, column: 42, scope: !536)
!552 = distinct !{!552, !541, !553}
!553 = !DILocation(line: 165, column: 11, scope: !537)
!554 = !DILocation(line: 170, column: 31, scope: !284)
!555 = !DILocation(line: 172, column: 46, scope: !556)
!556 = distinct !DILexicalBlock(scope: !283, file: !3, line: 172, column: 44)
!557 = !DILocation(line: 170, column: 13, scope: !285)
!558 = !DILocation(line: 175, column: 24, scope: !283)
!559 = !DILocation(line: 177, column: 48, scope: !283)
!560 = !DILocation(line: 177, column: 24, scope: !283)
!561 = !DILocation(line: 178, column: 15, scope: !562)
!562 = distinct !DILexicalBlock(scope: !283, file: !3, line: 178, column: 14)
!563 = !DILocation(line: 178, column: 14, scope: !283)
!564 = !DILocation(line: 181, column: 74, scope: !283)
!565 = !DILocation(line: 181, column: 61, scope: !283)
!566 = !DILocation(line: 181, column: 51, scope: !283)
!567 = !DILocation(line: 182, column: 45, scope: !568)
!568 = distinct !DILexicalBlock(scope: !283, file: !3, line: 182, column: 44)
!569 = !DILocation(line: 182, column: 44, scope: !283)
!570 = !DILocation(line: 185, column: 44, scope: !571)
!571 = distinct !DILexicalBlock(scope: !283, file: !3, line: 185, column: 44)
!572 = !DILocation(line: 185, column: 53, scope: !571)
!573 = !DILocation(line: 185, column: 44, scope: !283)
!574 = !DILocation(line: 186, column: 49, scope: !571)
!575 = !DILocation(line: 188, column: 41, scope: !283)
!576 = !DILocation(line: 190, column: 44, scope: !577)
!577 = distinct !DILexicalBlock(scope: !283, file: !3, line: 190, column: 44)
!578 = !DILocation(line: 190, column: 53, scope: !577)
!579 = !DILocation(line: 190, column: 44, scope: !283)
!580 = !DILocation(line: 191, column: 49, scope: !577)
!581 = !DILocation(line: 193, column: 45, scope: !283)
!582 = !DILocation(line: 193, column: 52, scope: !283)
!583 = !DILocation(line: 193, column: 16, scope: !283)
!584 = !DILocation(line: 170, column: 44, scope: !284)
!585 = distinct !{!585, !557, !586}
!586 = !DILocation(line: 194, column: 13, scope: !285)
!587 = !DILocation(line: 146, column: 17, scope: !288)
!588 = !DILocation(line: 147, column: 21, scope: !589)
!589 = distinct !DILexicalBlock(scope: !288, file: !3, line: 147, column: 14)
!590 = !DILocation(line: 205, column: 1, scope: !166)
!591 = distinct !DISubprogram(name: "_listener_tf", scope: !3, file: !3, line: 208, type: !592, isLocal: false, isDefinition: true, scopeLine: 208, flags: DIFlagPrototyped, isOptimized: true, unit: !2, variables: !602)
!592 = !DISubroutineType(types: !593)
!593 = !{!89, !594}
!594 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !595, size: 64)
!595 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "xdp_md", file: !15, line: 2859, size: 160, elements: !596)
!596 = !{!597, !598, !599, !600, !601}
!597 = !DIDerivedType(tag: DW_TAG_member, name: "data", scope: !595, file: !15, line: 2860, baseType: !87, size: 32)
!598 = !DIDerivedType(tag: DW_TAG_member, name: "data_end", scope: !595, file: !15, line: 2861, baseType: !87, size: 32, offset: 32)
!599 = !DIDerivedType(tag: DW_TAG_member, name: "data_meta", scope: !595, file: !15, line: 2862, baseType: !87, size: 32, offset: 64)
!600 = !DIDerivedType(tag: DW_TAG_member, name: "ingress_ifindex", scope: !595, file: !15, line: 2864, baseType: !87, size: 32, offset: 96)
!601 = !DIDerivedType(tag: DW_TAG_member, name: "rx_queue_index", scope: !595, file: !15, line: 2865, baseType: !87, size: 32, offset: 128)
!602 = !{!603, !604, !605, !606, !607, !608, !609, !610, !611}
!603 = !DILocalVariable(name: "ctx", arg: 1, scope: !591, file: !3, line: 208, type: !594)
!604 = !DILocalVariable(name: "index", scope: !591, file: !3, line: 210, type: !89)
!605 = !DILocalVariable(name: "sk_state", scope: !591, file: !3, line: 211, type: !83)
!606 = !DILocalVariable(name: "data", scope: !591, file: !3, line: 212, type: !23)
!607 = !DILocalVariable(name: "data_end", scope: !591, file: !3, line: 213, type: !23)
!608 = !DILocalVariable(name: "eth", scope: !591, file: !3, line: 215, type: !25)
!609 = !DILocalVariable(name: "iph", scope: !591, file: !3, line: 219, type: !41)
!610 = !DILocalVariable(name: "vtlh", scope: !591, file: !3, line: 226, type: !65)
!611 = !DILocalVariable(name: "ret", scope: !591, file: !3, line: 240, type: !24)
!612 = !DILocation(line: 208, column: 33, scope: !591)
!613 = !DILocation(line: 210, column: 4, scope: !591)
!614 = !DILocation(line: 210, column: 8, scope: !591)
!615 = !DILocation(line: 211, column: 25, scope: !591)
!616 = !DILocation(line: 212, column: 36, scope: !591)
!617 = !{!618, !342, i64 0}
!618 = !{!"xdp_md", !342, i64 0, !342, i64 4, !342, i64 8, !342, i64 12, !342, i64 16}
!619 = !DILocation(line: 212, column: 25, scope: !591)
!620 = !DILocation(line: 212, column: 10, scope: !591)
!621 = !DILocation(line: 213, column: 40, scope: !591)
!622 = !{!618, !342, i64 4}
!623 = !DILocation(line: 213, column: 29, scope: !591)
!624 = !DILocation(line: 213, column: 10, scope: !591)
!625 = !DILocation(line: 215, column: 25, scope: !591)
!626 = !DILocation(line: 215, column: 19, scope: !591)
!627 = !DILocation(line: 216, column: 11, scope: !628)
!628 = distinct !DILexicalBlock(scope: !591, file: !3, line: 216, column: 7)
!629 = !DILocation(line: 216, column: 17, scope: !628)
!630 = !DILocation(line: 216, column: 15, scope: !628)
!631 = !DILocation(line: 216, column: 7, scope: !591)
!632 = !DILocation(line: 219, column: 18, scope: !591)
!633 = !DILocation(line: 220, column: 11, scope: !634)
!634 = distinct !DILexicalBlock(scope: !591, file: !3, line: 220, column: 7)
!635 = !DILocation(line: 220, column: 17, scope: !634)
!636 = !DILocation(line: 220, column: 15, scope: !634)
!637 = !DILocation(line: 220, column: 7, scope: !591)
!638 = !DILocation(line: 223, column: 12, scope: !639)
!639 = distinct !DILexicalBlock(scope: !591, file: !3, line: 223, column: 7)
!640 = !DILocation(line: 223, column: 21, scope: !639)
!641 = !DILocation(line: 223, column: 7, scope: !591)
!642 = !DILocation(line: 226, column: 15, scope: !591)
!643 = !DILocation(line: 227, column: 12, scope: !644)
!644 = distinct !DILexicalBlock(scope: !591, file: !3, line: 227, column: 7)
!645 = !DILocation(line: 227, column: 18, scope: !644)
!646 = !DILocation(line: 227, column: 16, scope: !644)
!647 = !DILocation(line: 227, column: 7, scope: !591)
!648 = !DILocation(line: 230, column: 24, scope: !591)
!649 = !DILocation(line: 230, column: 12, scope: !591)
!650 = !DILocation(line: 230, column: 10, scope: !591)
!651 = !DILocation(line: 231, column: 39, scope: !591)
!652 = !DILocation(line: 232, column: 8, scope: !653)
!653 = distinct !DILexicalBlock(scope: !591, file: !3, line: 232, column: 7)
!654 = !DILocation(line: 232, column: 7, scope: !591)
!655 = !DILocation(line: 235, column: 13, scope: !656)
!656 = distinct !DILexicalBlock(scope: !591, file: !3, line: 235, column: 7)
!657 = !DILocation(line: 235, column: 22, scope: !656)
!658 = !DILocation(line: 236, column: 7, scope: !656)
!659 = !DILocation(line: 240, column: 15, scope: !591)
!660 = !DILocation(line: 245, column: 1, scope: !591)
