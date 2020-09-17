; ModuleID = 'ingress_test_tf.c'
source_filename = "ingress_test_tf.c"
target datalayout = "e-m:e-p:64:64-i64:64-n32:64-S128"
target triple = "bpf"

%struct.bpf_map_def = type { i32, i32, i32, i32, i32, i32, i32 }
%struct.xdp_md = type { i32, i32, i32, i32, i32 }
%struct.ethhdr = type { [6 x i8], [6 x i8], i16 }
%struct.iphdr = type { i8, i8, i16, i16, i16, i8, i8, i16, i32, i32 }
%struct.vtl_hdr_t = type { i8, i32, i16, i16 }

@xsks_map = global %struct.bpf_map_def { i32 17, i32 4, i32 4, i32 64, i32 0, i32 0, i32 0 }, section "maps", align 4, !dbg !0
@QOS_NEGO_MAP = global %struct.bpf_map_def { i32 2, i32 4, i32 4, i32 1, i32 0, i32 0, i32 0 }, section "maps", align 4, !dbg !86
@xdp_sock_prog.____fmt = private unnamed_addr constant [24 x i8] c"ETH: malformed header.\0A\00", align 1
@xdp_sock_prog.____fmt.1 = private unnamed_addr constant [24 x i8] c"IPH: malformed header.\0A\00", align 1
@xdp_sock_prog.____fmt.2 = private unnamed_addr constant [25 x i8] c"VTLH: malformed header.\0A\00", align 1
@xdp_sock_prog.____fmt.3 = private unnamed_addr constant [40 x i8] c"Can not lookup QOS_NEGO_MAP. Skipping.\0A\00", align 1
@_license = global [4 x i8] c"GPL\00", section "license", align 1, !dbg !98
@llvm.used = appending global [4 x i8*] [i8* bitcast (%struct.bpf_map_def* @QOS_NEGO_MAP to i8*), i8* getelementptr inbounds ([4 x i8], [4 x i8]* @_license, i32 0, i32 0), i8* bitcast (i32 (%struct.xdp_md*)* @xdp_sock_prog to i8*), i8* bitcast (%struct.bpf_map_def* @xsks_map to i8*)], section "llvm.metadata"

; Function Attrs: nounwind
define i32 @xdp_sock_prog(%struct.xdp_md* nocapture readonly) #0 section "ingress_tf_sec" !dbg !132 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca [24 x i8], align 1
  %5 = alloca [24 x i8], align 1
  %6 = alloca [25 x i8], align 1
  %7 = alloca [40 x i8], align 1
  call void @llvm.dbg.value(metadata %struct.xdp_md* %0, metadata !145, metadata !DIExpression()), !dbg !181
  %8 = bitcast i32* %2 to i8*, !dbg !182
  call void @llvm.lifetime.start.p0i8(i64 4, i8* nonnull %8) #3, !dbg !182
  %9 = getelementptr inbounds %struct.xdp_md, %struct.xdp_md* %0, i64 0, i32 4, !dbg !183
  %10 = load i32, i32* %9, align 4, !dbg !183, !tbaa !184
  call void @llvm.dbg.value(metadata i32 %10, metadata !146, metadata !DIExpression()), !dbg !189
  store i32 %10, i32* %2, align 4, !dbg !189, !tbaa !190
  %11 = bitcast i32* %3 to i8*, !dbg !182
  call void @llvm.lifetime.start.p0i8(i64 4, i8* nonnull %11) #3, !dbg !182
  call void @llvm.dbg.value(metadata i32 0, metadata !147, metadata !DIExpression()), !dbg !191
  store i32 0, i32* %3, align 4, !dbg !191, !tbaa !190
  call void @llvm.dbg.value(metadata i32* null, metadata !148, metadata !DIExpression()), !dbg !192
  %12 = getelementptr inbounds %struct.xdp_md, %struct.xdp_md* %0, i64 0, i32 0, !dbg !193
  %13 = load i32, i32* %12, align 4, !dbg !193, !tbaa !194
  %14 = zext i32 %13 to i64, !dbg !195
  call void @llvm.dbg.value(metadata i64 %14, metadata !149, metadata !DIExpression()), !dbg !196
  %15 = getelementptr inbounds %struct.xdp_md, %struct.xdp_md* %0, i64 0, i32 1, !dbg !197
  %16 = load i32, i32* %15, align 4, !dbg !197, !tbaa !198
  %17 = zext i32 %16 to i64, !dbg !199
  call void @llvm.dbg.value(metadata i64 %17, metadata !150, metadata !DIExpression()), !dbg !200
  %18 = inttoptr i64 %14 to %struct.ethhdr*, !dbg !201
  call void @llvm.dbg.value(metadata %struct.ethhdr* %18, metadata !151, metadata !DIExpression()), !dbg !202
  %19 = getelementptr inbounds %struct.ethhdr, %struct.ethhdr* %18, i64 1, !dbg !203
  %20 = inttoptr i64 %17 to %struct.ethhdr*, !dbg !204
  %21 = icmp ugt %struct.ethhdr* %19, %20, !dbg !205
  br i1 %21, label %22, label %25, !dbg !206

; <label>:22:                                     ; preds = %1
  %23 = getelementptr inbounds [24 x i8], [24 x i8]* %4, i64 0, i64 0, !dbg !207
  call void @llvm.lifetime.start.p0i8(i64 24, i8* nonnull %23) #3, !dbg !207
  call void @llvm.dbg.declare(metadata [24 x i8]* %4, metadata !152, metadata !DIExpression()), !dbg !207
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* nonnull %23, i8* getelementptr inbounds ([24 x i8], [24 x i8]* @xdp_sock_prog.____fmt, i64 0, i64 0), i64 24, i32 1, i1 false), !dbg !207
  %24 = call i32 (i8*, i32, ...) inttoptr (i64 7 to i32 (i8*, i32, ...)*)(i8* nonnull %23, i32 24) #3, !dbg !207
  call void @llvm.lifetime.end.p0i8(i64 24, i8* nonnull %23) #3, !dbg !208
  br label %77, !dbg !209

; <label>:25:                                     ; preds = %1
  call void @llvm.dbg.value(metadata %struct.ethhdr* %19, metadata !159, metadata !DIExpression()), !dbg !210
  %26 = getelementptr inbounds %struct.ethhdr, %struct.ethhdr* %18, i64 2, i32 1, !dbg !211
  %27 = bitcast [6 x i8]* %26 to %struct.iphdr*, !dbg !211
  %28 = inttoptr i64 %17 to %struct.iphdr*, !dbg !212
  %29 = icmp ugt %struct.iphdr* %27, %28, !dbg !213
  br i1 %29, label %30, label %33, !dbg !214

; <label>:30:                                     ; preds = %25
  %31 = getelementptr inbounds [24 x i8], [24 x i8]* %5, i64 0, i64 0, !dbg !215
  call void @llvm.lifetime.start.p0i8(i64 24, i8* nonnull %31) #3, !dbg !215
  call void @llvm.dbg.declare(metadata [24 x i8]* %5, metadata !160, metadata !DIExpression()), !dbg !215
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* nonnull %31, i8* getelementptr inbounds ([24 x i8], [24 x i8]* @xdp_sock_prog.____fmt.1, i64 0, i64 0), i64 24, i32 1, i1 false), !dbg !215
  %32 = call i32 (i8*, i32, ...) inttoptr (i64 7 to i32 (i8*, i32, ...)*)(i8* nonnull %31, i32 24) #3, !dbg !215
  call void @llvm.lifetime.end.p0i8(i64 24, i8* nonnull %31) #3, !dbg !216
  br label %77, !dbg !217

; <label>:33:                                     ; preds = %25
  %34 = getelementptr inbounds %struct.ethhdr, %struct.ethhdr* %18, i64 1, i32 1, i64 3, !dbg !218
  %35 = load i8, i8* %34, align 1, !dbg !218, !tbaa !220
  %36 = icmp eq i8 %35, -3, !dbg !223
  br i1 %36, label %37, label %77, !dbg !224

; <label>:37:                                     ; preds = %33
  call void @llvm.dbg.value(metadata [6 x i8]* %26, metadata !164, metadata !DIExpression()), !dbg !225
  %38 = getelementptr inbounds [6 x i8], [6 x i8]* %26, i64 2, !dbg !226
  %39 = bitcast [6 x i8]* %38 to %struct.vtl_hdr_t*, !dbg !226
  %40 = inttoptr i64 %17 to %struct.vtl_hdr_t*, !dbg !227
  %41 = icmp ugt %struct.vtl_hdr_t* %39, %40, !dbg !228
  br i1 %41, label %42, label %45, !dbg !229

; <label>:42:                                     ; preds = %37
  %43 = getelementptr inbounds [25 x i8], [25 x i8]* %6, i64 0, i64 0, !dbg !230
  call void @llvm.lifetime.start.p0i8(i64 25, i8* nonnull %43) #3, !dbg !230
  call void @llvm.dbg.declare(metadata [25 x i8]* %6, metadata !165, metadata !DIExpression()), !dbg !230
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* nonnull %43, i8* getelementptr inbounds ([25 x i8], [25 x i8]* @xdp_sock_prog.____fmt.2, i64 0, i64 0), i64 25, i32 1, i1 false), !dbg !230
  %44 = call i32 (i8*, i32, ...) inttoptr (i64 7 to i32 (i8*, i32, ...)*)(i8* nonnull %43, i32 25) #3, !dbg !230
  call void @llvm.lifetime.end.p0i8(i64 25, i8* nonnull %43) #3, !dbg !231
  br label %77, !dbg !232

; <label>:45:                                     ; preds = %37
  %46 = getelementptr inbounds [6 x i8], [6 x i8]* %26, i64 0, i64 0, !dbg !233
  %47 = load i8, i8* %46, align 4, !dbg !233, !tbaa !234
  %48 = icmp sgt i8 %47, 0, !dbg !236
  br i1 %48, label %49, label %69, !dbg !237

; <label>:49:                                     ; preds = %45
  tail call void inttoptr (i64 6 to void (i64)*)(i64 10) #3, !dbg !238
  %50 = call i8* inttoptr (i64 1 to i8* (i8*, i8*)*)(i8* bitcast (%struct.bpf_map_def* @QOS_NEGO_MAP to i8*), i8* nonnull %11) #3, !dbg !239
  call void @llvm.dbg.value(metadata i32* %56, metadata !148, metadata !DIExpression()), !dbg !192
  %51 = icmp eq i8* %50, null, !dbg !240
  br i1 %51, label %52, label %55, !dbg !241

; <label>:52:                                     ; preds = %49
  %53 = getelementptr inbounds [40 x i8], [40 x i8]* %7, i64 0, i64 0, !dbg !242
  call void @llvm.lifetime.start.p0i8(i64 40, i8* nonnull %53) #3, !dbg !242
  call void @llvm.dbg.declare(metadata [40 x i8]* %7, metadata !172, metadata !DIExpression()), !dbg !242
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* nonnull %53, i8* getelementptr inbounds ([40 x i8], [40 x i8]* @xdp_sock_prog.____fmt.3, i64 0, i64 0), i64 40, i32 1, i1 false), !dbg !242
  %54 = call i32 (i8*, i32, ...) inttoptr (i64 7 to i32 (i8*, i32, ...)*)(i8* nonnull %53, i32 40) #3, !dbg !242
  call void @llvm.lifetime.end.p0i8(i64 40, i8* nonnull %53) #3, !dbg !243
  br label %77, !dbg !244

; <label>:55:                                     ; preds = %49
  %56 = bitcast i8* %50 to i32*, !dbg !245
  %57 = load i32, i32* %56, align 4, !dbg !246, !tbaa !248
  switch i32 %57, label %63 [
    i32 0, label %59
    i32 1, label %58
  ], !dbg !249

; <label>:58:                                     ; preds = %55
  br label %59, !dbg !250

; <label>:59:                                     ; preds = %55, %58
  %60 = phi i32 [ 5, %58 ], [ 4, %55 ]
  %61 = getelementptr inbounds %struct.ethhdr, %struct.ethhdr* %18, i64 2, i32 1, i64 4
  %62 = bitcast i8* %61 to i32*
  store i32 %60, i32* %62, align 4, !tbaa !253
  br label %63, !dbg !254

; <label>:63:                                     ; preds = %59, %55
  %64 = call i8* inttoptr (i64 1 to i8* (i8*, i8*)*)(i8* bitcast (%struct.bpf_map_def* @xsks_map to i8*), i8* nonnull %8) #3, !dbg !254
  %65 = icmp eq i8* %64, null, !dbg !254
  br i1 %65, label %77, label %66, !dbg !256

; <label>:66:                                     ; preds = %63
  %67 = load i32, i32* %2, align 4, !dbg !257, !tbaa !190
  call void @llvm.dbg.value(metadata i32 %67, metadata !146, metadata !DIExpression()), !dbg !189
  %68 = call i32 inttoptr (i64 53 to i32 (i8*, i32, i32)*)(i8* bitcast (%struct.bpf_map_def* @xsks_map to i8*), i32 %67, i32 0) #3, !dbg !258
  br label %77, !dbg !259

; <label>:69:                                     ; preds = %45
  %70 = icmp eq i8 %47, 0, !dbg !260
  br i1 %70, label %71, label %77, !dbg !262

; <label>:71:                                     ; preds = %69
  %72 = call i8* inttoptr (i64 1 to i8* (i8*, i8*)*)(i8* bitcast (%struct.bpf_map_def* @xsks_map to i8*), i8* nonnull %8) #3, !dbg !263
  %73 = icmp eq i8* %72, null, !dbg !263
  br i1 %73, label %77, label %74, !dbg !266

; <label>:74:                                     ; preds = %71
  %75 = load i32, i32* %2, align 4, !dbg !267, !tbaa !190
  call void @llvm.dbg.value(metadata i32 %75, metadata !146, metadata !DIExpression()), !dbg !189
  %76 = call i32 inttoptr (i64 53 to i32 (i8*, i32, i32)*)(i8* bitcast (%struct.bpf_map_def* @xsks_map to i8*), i32 %75, i32 0) #3, !dbg !268
  br label %77, !dbg !269

; <label>:77:                                     ; preds = %30, %33, %69, %71, %63, %74, %66, %52, %42, %22
  %78 = phi i32 [ 1, %22 ], [ 1, %30 ], [ 2, %33 ], [ 1, %42 ], [ %68, %66 ], [ 1, %52 ], [ %76, %74 ], [ 2, %63 ], [ 2, %71 ], [ 2, %69 ]
  call void @llvm.lifetime.end.p0i8(i64 4, i8* nonnull %11) #3, !dbg !270
  call void @llvm.lifetime.end.p0i8(i64 4, i8* nonnull %8) #3, !dbg !270
  ret i32 %78, !dbg !270
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.start.p0i8(i64, i8* nocapture) #2

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i32, i1) #2

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.end.p0i8(i64, i8* nocapture) #2

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.value(metadata, metadata, metadata) #1

attributes #0 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { argmemonly nounwind }
attributes #3 = { nounwind }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!128, !129, !130}
!llvm.ident = !{!131}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "xsks_map", scope: !2, file: !3, line: 13, type: !88, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 6.0.0-1ubuntu2 (tags/RELEASE_600/final)", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug, enums: !4, retainedTypes: !27, globals: !85)
!3 = !DIFile(filename: "ingress_test_tf.c", directory: "/home/vtl_server/vtlWorkspace/vtl_server/XTFsPool")
!4 = !{!5, !11, !19}
!5 = !DICompositeType(tag: DW_TAG_enumeration_type, file: !6, line: 49, size: 32, elements: !7)
!6 = !DIFile(filename: "./../include/vtl.h", directory: "/home/vtl_server/vtlWorkspace/vtl_server/XTFsPool")
!7 = !{!8, !9, !10}
!8 = !DIEnumerator(name: "N_ACCEPT", value: 0)
!9 = !DIEnumerator(name: "N_REFUSE", value: 1)
!10 = !DIEnumerator(name: "N_IDLE", value: 2)
!11 = !DICompositeType(tag: DW_TAG_enumeration_type, name: "xdp_action", file: !12, line: 2847, size: 32, elements: !13)
!12 = !DIFile(filename: "./include/linux/bpf.h", directory: "/home/vtl_server/vtlWorkspace/vtl_server/XTFsPool")
!13 = !{!14, !15, !16, !17, !18}
!14 = !DIEnumerator(name: "XDP_ABORTED", value: 0)
!15 = !DIEnumerator(name: "XDP_DROP", value: 1)
!16 = !DIEnumerator(name: "XDP_PASS", value: 2)
!17 = !DIEnumerator(name: "XDP_TX", value: 3)
!18 = !DIEnumerator(name: "XDP_REDIRECT", value: 4)
!19 = !DICompositeType(tag: DW_TAG_enumeration_type, file: !6, line: 64, size: 32, elements: !20)
!20 = !{!21, !22, !23, !24, !25, !26}
!21 = !DIEnumerator(name: "NEGO", value: 0)
!22 = !DIEnumerator(name: "DATA", value: 1)
!23 = !DIEnumerator(name: "ACK", value: 2)
!24 = !DIEnumerator(name: "NACK", value: 3)
!25 = !DIEnumerator(name: "NEGO_ACK", value: 4)
!26 = !DIEnumerator(name: "NEGO_NACK", value: 5)
!27 = !{!28, !29, !30, !46, !70, !83}
!28 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: 64)
!29 = !DIBasicType(name: "long int", size: 64, encoding: DW_ATE_signed)
!30 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !31, size: 64)
!31 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "ethhdr", file: !32, line: 163, size: 112, elements: !33)
!32 = !DIFile(filename: "./include/linux/if_ether.h", directory: "/home/vtl_server/vtlWorkspace/vtl_server/XTFsPool")
!33 = !{!34, !39, !40}
!34 = !DIDerivedType(tag: DW_TAG_member, name: "h_dest", scope: !31, file: !32, line: 164, baseType: !35, size: 48)
!35 = !DICompositeType(tag: DW_TAG_array_type, baseType: !36, size: 48, elements: !37)
!36 = !DIBasicType(name: "unsigned char", size: 8, encoding: DW_ATE_unsigned_char)
!37 = !{!38}
!38 = !DISubrange(count: 6)
!39 = !DIDerivedType(tag: DW_TAG_member, name: "h_source", scope: !31, file: !32, line: 165, baseType: !35, size: 48, offset: 48)
!40 = !DIDerivedType(tag: DW_TAG_member, name: "h_proto", scope: !31, file: !32, line: 166, baseType: !41, size: 16, offset: 96)
!41 = !DIDerivedType(tag: DW_TAG_typedef, name: "__be16", file: !42, line: 25, baseType: !43)
!42 = !DIFile(filename: "/usr/include/linux/types.h", directory: "/home/vtl_server/vtlWorkspace/vtl_server/XTFsPool")
!43 = !DIDerivedType(tag: DW_TAG_typedef, name: "__u16", file: !44, line: 24, baseType: !45)
!44 = !DIFile(filename: "/usr/include/asm-generic/int-ll64.h", directory: "/home/vtl_server/vtlWorkspace/vtl_server/XTFsPool")
!45 = !DIBasicType(name: "unsigned short", size: 16, encoding: DW_ATE_unsigned)
!46 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !47, size: 64)
!47 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "iphdr", file: !48, line: 44, size: 160, elements: !49)
!48 = !DIFile(filename: "/usr/include/netinet/ip.h", directory: "/home/vtl_server/vtlWorkspace/vtl_server/XTFsPool")
!49 = !{!50, !52, !53, !58, !61, !62, !63, !64, !65, !66, !69}
!50 = !DIDerivedType(tag: DW_TAG_member, name: "ihl", scope: !47, file: !48, line: 47, baseType: !51, size: 4, flags: DIFlagBitField, extraData: i64 0)
!51 = !DIBasicType(name: "unsigned int", size: 32, encoding: DW_ATE_unsigned)
!52 = !DIDerivedType(tag: DW_TAG_member, name: "version", scope: !47, file: !48, line: 48, baseType: !51, size: 4, offset: 4, flags: DIFlagBitField, extraData: i64 0)
!53 = !DIDerivedType(tag: DW_TAG_member, name: "tos", scope: !47, file: !48, line: 55, baseType: !54, size: 8, offset: 8)
!54 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint8_t", file: !55, line: 24, baseType: !56)
!55 = !DIFile(filename: "/usr/include/bits/stdint-uintn.h", directory: "/home/vtl_server/vtlWorkspace/vtl_server/XTFsPool")
!56 = !DIDerivedType(tag: DW_TAG_typedef, name: "__uint8_t", file: !57, line: 37, baseType: !36)
!57 = !DIFile(filename: "/usr/include/bits/types.h", directory: "/home/vtl_server/vtlWorkspace/vtl_server/XTFsPool")
!58 = !DIDerivedType(tag: DW_TAG_member, name: "tot_len", scope: !47, file: !48, line: 56, baseType: !59, size: 16, offset: 16)
!59 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint16_t", file: !55, line: 25, baseType: !60)
!60 = !DIDerivedType(tag: DW_TAG_typedef, name: "__uint16_t", file: !57, line: 39, baseType: !45)
!61 = !DIDerivedType(tag: DW_TAG_member, name: "id", scope: !47, file: !48, line: 57, baseType: !59, size: 16, offset: 32)
!62 = !DIDerivedType(tag: DW_TAG_member, name: "frag_off", scope: !47, file: !48, line: 58, baseType: !59, size: 16, offset: 48)
!63 = !DIDerivedType(tag: DW_TAG_member, name: "ttl", scope: !47, file: !48, line: 59, baseType: !54, size: 8, offset: 64)
!64 = !DIDerivedType(tag: DW_TAG_member, name: "protocol", scope: !47, file: !48, line: 60, baseType: !54, size: 8, offset: 72)
!65 = !DIDerivedType(tag: DW_TAG_member, name: "check", scope: !47, file: !48, line: 61, baseType: !59, size: 16, offset: 80)
!66 = !DIDerivedType(tag: DW_TAG_member, name: "saddr", scope: !47, file: !48, line: 62, baseType: !67, size: 32, offset: 96)
!67 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint32_t", file: !55, line: 26, baseType: !68)
!68 = !DIDerivedType(tag: DW_TAG_typedef, name: "__uint32_t", file: !57, line: 41, baseType: !51)
!69 = !DIDerivedType(tag: DW_TAG_member, name: "daddr", scope: !47, file: !48, line: 63, baseType: !67, size: 32, offset: 128)
!70 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !71, size: 64)
!71 = !DIDerivedType(tag: DW_TAG_typedef, name: "vtl_hdr_t", file: !6, line: 81, baseType: !72)
!72 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !6, line: 76, size: 96, elements: !73)
!73 = !{!74, !79, !81, !82}
!74 = !DIDerivedType(tag: DW_TAG_member, name: "gid", scope: !72, file: !6, line: 77, baseType: !75, size: 8)
!75 = !DIDerivedType(tag: DW_TAG_typedef, name: "int8_t", file: !76, line: 24, baseType: !77)
!76 = !DIFile(filename: "/usr/include/bits/stdint-intn.h", directory: "/home/vtl_server/vtlWorkspace/vtl_server/XTFsPool")
!77 = !DIDerivedType(tag: DW_TAG_typedef, name: "__int8_t", file: !57, line: 36, baseType: !78)
!78 = !DIBasicType(name: "signed char", size: 8, encoding: DW_ATE_signed_char)
!79 = !DIDerivedType(tag: DW_TAG_member, name: "pkt_type", scope: !72, file: !6, line: 78, baseType: !80, size: 32, offset: 32)
!80 = !DIDerivedType(tag: DW_TAG_typedef, name: "vtl_pkt_type", file: !6, line: 71, baseType: !19)
!81 = !DIDerivedType(tag: DW_TAG_member, name: "checksum", scope: !72, file: !6, line: 79, baseType: !59, size: 16, offset: 64)
!82 = !DIDerivedType(tag: DW_TAG_member, name: "seq_num", scope: !72, file: !6, line: 80, baseType: !59, size: 16, offset: 80)
!83 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !84, size: 64)
!84 = !DIDerivedType(tag: DW_TAG_typedef, name: "negotiation_state", file: !6, line: 53, baseType: !5)
!85 = !{!0, !86, !98, !104, !112, !118, !123}
!86 = !DIGlobalVariableExpression(var: !87, expr: !DIExpression())
!87 = distinct !DIGlobalVariable(name: "QOS_NEGO_MAP", scope: !2, file: !3, line: 20, type: !88, isLocal: false, isDefinition: true)
!88 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "bpf_map_def", file: !89, line: 215, size: 224, elements: !90)
!89 = !DIFile(filename: "./include/bpf/bpf_helpers.h", directory: "/home/vtl_server/vtlWorkspace/vtl_server/XTFsPool")
!90 = !{!91, !92, !93, !94, !95, !96, !97}
!91 = !DIDerivedType(tag: DW_TAG_member, name: "type", scope: !88, file: !89, line: 216, baseType: !51, size: 32)
!92 = !DIDerivedType(tag: DW_TAG_member, name: "key_size", scope: !88, file: !89, line: 217, baseType: !51, size: 32, offset: 32)
!93 = !DIDerivedType(tag: DW_TAG_member, name: "value_size", scope: !88, file: !89, line: 218, baseType: !51, size: 32, offset: 64)
!94 = !DIDerivedType(tag: DW_TAG_member, name: "max_entries", scope: !88, file: !89, line: 219, baseType: !51, size: 32, offset: 96)
!95 = !DIDerivedType(tag: DW_TAG_member, name: "map_flags", scope: !88, file: !89, line: 220, baseType: !51, size: 32, offset: 128)
!96 = !DIDerivedType(tag: DW_TAG_member, name: "inner_map_idx", scope: !88, file: !89, line: 221, baseType: !51, size: 32, offset: 160)
!97 = !DIDerivedType(tag: DW_TAG_member, name: "numa_node", scope: !88, file: !89, line: 222, baseType: !51, size: 32, offset: 192)
!98 = !DIGlobalVariableExpression(var: !99, expr: !DIExpression())
!99 = distinct !DIGlobalVariable(name: "_license", scope: !2, file: !3, line: 87, type: !100, isLocal: false, isDefinition: true)
!100 = !DICompositeType(tag: DW_TAG_array_type, baseType: !101, size: 32, elements: !102)
!101 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!102 = !{!103}
!103 = !DISubrange(count: 4)
!104 = !DIGlobalVariableExpression(var: !105, expr: !DIExpression())
!105 = distinct !DIGlobalVariable(name: "bpf_trace_printk", scope: !2, file: !89, line: 40, type: !106, isLocal: true, isDefinition: true)
!106 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !107, size: 64)
!107 = !DISubroutineType(types: !108)
!108 = !{!109, !110, !109, null}
!109 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!110 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !111, size: 64)
!111 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !101)
!112 = !DIGlobalVariableExpression(var: !113, expr: !DIExpression())
!113 = distinct !DIGlobalVariable(name: "bpf_vtl_start_timer", scope: !2, file: !89, line: 38, type: !114, isLocal: true, isDefinition: true)
!114 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !115, size: 64)
!115 = !DISubroutineType(types: !116)
!116 = !{null, !117}
!117 = !DIBasicType(name: "long long unsigned int", size: 64, encoding: DW_ATE_unsigned)
!118 = !DIGlobalVariableExpression(var: !119, expr: !DIExpression())
!119 = distinct !DIGlobalVariable(name: "bpf_map_lookup_elem", scope: !2, file: !89, line: 20, type: !120, isLocal: true, isDefinition: true)
!120 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !121, size: 64)
!121 = !DISubroutineType(types: !122)
!122 = !{!28, !28, !28}
!123 = !DIGlobalVariableExpression(var: !124, expr: !DIExpression())
!124 = distinct !DIGlobalVariable(name: "bpf_redirect_map", scope: !2, file: !89, line: 62, type: !125, isLocal: true, isDefinition: true)
!125 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !126, size: 64)
!126 = !DISubroutineType(types: !127)
!127 = !{!109, !28, !109, !109}
!128 = !{i32 2, !"Dwarf Version", i32 4}
!129 = !{i32 2, !"Debug Info Version", i32 3}
!130 = !{i32 1, !"wchar_size", i32 4}
!131 = !{!"clang version 6.0.0-1ubuntu2 (tags/RELEASE_600/final)"}
!132 = distinct !DISubprogram(name: "xdp_sock_prog", scope: !3, file: !3, line: 28, type: !133, isLocal: false, isDefinition: true, scopeLine: 29, flags: DIFlagPrototyped, isOptimized: true, unit: !2, variables: !144)
!133 = !DISubroutineType(types: !134)
!134 = !{!109, !135}
!135 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !136, size: 64)
!136 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "xdp_md", file: !12, line: 2858, size: 160, elements: !137)
!137 = !{!138, !140, !141, !142, !143}
!138 = !DIDerivedType(tag: DW_TAG_member, name: "data", scope: !136, file: !12, line: 2859, baseType: !139, size: 32)
!139 = !DIDerivedType(tag: DW_TAG_typedef, name: "__u32", file: !44, line: 27, baseType: !51)
!140 = !DIDerivedType(tag: DW_TAG_member, name: "data_end", scope: !136, file: !12, line: 2860, baseType: !139, size: 32, offset: 32)
!141 = !DIDerivedType(tag: DW_TAG_member, name: "data_meta", scope: !136, file: !12, line: 2861, baseType: !139, size: 32, offset: 64)
!142 = !DIDerivedType(tag: DW_TAG_member, name: "ingress_ifindex", scope: !136, file: !12, line: 2863, baseType: !139, size: 32, offset: 96)
!143 = !DIDerivedType(tag: DW_TAG_member, name: "rx_queue_index", scope: !136, file: !12, line: 2864, baseType: !139, size: 32, offset: 128)
!144 = !{!145, !146, !147, !148, !149, !150, !151, !152, !159, !160, !164, !165, !172}
!145 = !DILocalVariable(name: "ctx", arg: 1, scope: !132, file: !3, line: 28, type: !135)
!146 = !DILocalVariable(name: "index", scope: !132, file: !3, line: 30, type: !109)
!147 = !DILocalVariable(name: "id", scope: !132, file: !3, line: 30, type: !109)
!148 = !DILocalVariable(name: "nego_state", scope: !132, file: !3, line: 31, type: !83)
!149 = !DILocalVariable(name: "data", scope: !132, file: !3, line: 33, type: !28)
!150 = !DILocalVariable(name: "data_end", scope: !132, file: !3, line: 34, type: !28)
!151 = !DILocalVariable(name: "eth", scope: !132, file: !3, line: 36, type: !30)
!152 = !DILocalVariable(name: "____fmt", scope: !153, file: !3, line: 38, type: !156)
!153 = distinct !DILexicalBlock(scope: !154, file: !3, line: 38, column: 9)
!154 = distinct !DILexicalBlock(scope: !155, file: !3, line: 37, column: 28)
!155 = distinct !DILexicalBlock(scope: !132, file: !3, line: 37, column: 9)
!156 = !DICompositeType(tag: DW_TAG_array_type, baseType: !101, size: 192, elements: !157)
!157 = !{!158}
!158 = !DISubrange(count: 24)
!159 = !DILocalVariable(name: "iph", scope: !132, file: !3, line: 42, type: !46)
!160 = !DILocalVariable(name: "____fmt", scope: !161, file: !3, line: 44, type: !156)
!161 = distinct !DILexicalBlock(scope: !162, file: !3, line: 44, column: 9)
!162 = distinct !DILexicalBlock(scope: !163, file: !3, line: 43, column: 28)
!163 = distinct !DILexicalBlock(scope: !132, file: !3, line: 43, column: 9)
!164 = !DILocalVariable(name: "vtlh", scope: !132, file: !3, line: 52, type: !70)
!165 = !DILocalVariable(name: "____fmt", scope: !166, file: !3, line: 54, type: !169)
!166 = distinct !DILexicalBlock(scope: !167, file: !3, line: 54, column: 9)
!167 = distinct !DILexicalBlock(scope: !168, file: !3, line: 53, column: 28)
!168 = distinct !DILexicalBlock(scope: !132, file: !3, line: 53, column: 8)
!169 = !DICompositeType(tag: DW_TAG_array_type, baseType: !101, size: 200, elements: !170)
!170 = !{!171}
!171 = !DISubrange(count: 25)
!172 = !DILocalVariable(name: "____fmt", scope: !173, file: !3, line: 64, type: !178)
!173 = distinct !DILexicalBlock(scope: !174, file: !3, line: 64, column: 9)
!174 = distinct !DILexicalBlock(scope: !175, file: !3, line: 63, column: 21)
!175 = distinct !DILexicalBlock(scope: !176, file: !3, line: 63, column: 8)
!176 = distinct !DILexicalBlock(scope: !177, file: !3, line: 58, column: 23)
!177 = distinct !DILexicalBlock(scope: !132, file: !3, line: 58, column: 8)
!178 = !DICompositeType(tag: DW_TAG_array_type, baseType: !101, size: 320, elements: !179)
!179 = !{!180}
!180 = !DISubrange(count: 40)
!181 = !DILocation(line: 28, column: 34, scope: !132)
!182 = !DILocation(line: 30, column: 5, scope: !132)
!183 = !DILocation(line: 30, column: 22, scope: !132)
!184 = !{!185, !186, i64 16}
!185 = !{!"xdp_md", !186, i64 0, !186, i64 4, !186, i64 8, !186, i64 12, !186, i64 16}
!186 = !{!"int", !187, i64 0}
!187 = !{!"omnipotent char", !188, i64 0}
!188 = !{!"Simple C/C++ TBAA"}
!189 = !DILocation(line: 30, column: 9, scope: !132)
!190 = !{!186, !186, i64 0}
!191 = !DILocation(line: 30, column: 38, scope: !132)
!192 = !DILocation(line: 31, column: 24, scope: !132)
!193 = !DILocation(line: 33, column: 37, scope: !132)
!194 = !{!185, !186, i64 0}
!195 = !DILocation(line: 33, column: 26, scope: !132)
!196 = !DILocation(line: 33, column: 11, scope: !132)
!197 = !DILocation(line: 34, column: 41, scope: !132)
!198 = !{!185, !186, i64 4}
!199 = !DILocation(line: 34, column: 30, scope: !132)
!200 = !DILocation(line: 34, column: 11, scope: !132)
!201 = !DILocation(line: 36, column: 26, scope: !132)
!202 = !DILocation(line: 36, column: 20, scope: !132)
!203 = !DILocation(line: 37, column: 13, scope: !155)
!204 = !DILocation(line: 37, column: 19, scope: !155)
!205 = !DILocation(line: 37, column: 17, scope: !155)
!206 = !DILocation(line: 37, column: 9, scope: !132)
!207 = !DILocation(line: 38, column: 9, scope: !153)
!208 = !DILocation(line: 38, column: 9, scope: !154)
!209 = !DILocation(line: 39, column: 9, scope: !154)
!210 = !DILocation(line: 42, column: 19, scope: !132)
!211 = !DILocation(line: 43, column: 13, scope: !163)
!212 = !DILocation(line: 43, column: 19, scope: !163)
!213 = !DILocation(line: 43, column: 17, scope: !163)
!214 = !DILocation(line: 43, column: 9, scope: !132)
!215 = !DILocation(line: 44, column: 9, scope: !161)
!216 = !DILocation(line: 44, column: 9, scope: !162)
!217 = !DILocation(line: 45, column: 9, scope: !162)
!218 = !DILocation(line: 48, column: 13, scope: !219)
!219 = distinct !DILexicalBlock(scope: !132, file: !3, line: 48, column: 8)
!220 = !{!221, !187, i64 9}
!221 = !{!"iphdr", !186, i64 0, !186, i64 0, !187, i64 1, !222, i64 2, !222, i64 4, !222, i64 6, !187, i64 8, !187, i64 9, !222, i64 10, !186, i64 12, !186, i64 16}
!222 = !{!"short", !187, i64 0}
!223 = !DILocation(line: 48, column: 22, scope: !219)
!224 = !DILocation(line: 48, column: 8, scope: !132)
!225 = !DILocation(line: 52, column: 16, scope: !132)
!226 = !DILocation(line: 53, column: 13, scope: !168)
!227 = !DILocation(line: 53, column: 19, scope: !168)
!228 = !DILocation(line: 53, column: 17, scope: !168)
!229 = !DILocation(line: 53, column: 8, scope: !132)
!230 = !DILocation(line: 54, column: 9, scope: !166)
!231 = !DILocation(line: 54, column: 9, scope: !167)
!232 = !DILocation(line: 55, column: 9, scope: !167)
!233 = !DILocation(line: 58, column: 14, scope: !177)
!234 = !{!235, !187, i64 0}
!235 = !{!"", !187, i64 0, !187, i64 4, !222, i64 8, !222, i64 10}
!236 = !DILocation(line: 58, column: 18, scope: !177)
!237 = !DILocation(line: 58, column: 8, scope: !132)
!238 = !DILocation(line: 60, column: 9, scope: !176)
!239 = !DILocation(line: 62, column: 40, scope: !176)
!240 = !DILocation(line: 63, column: 9, scope: !175)
!241 = !DILocation(line: 63, column: 8, scope: !176)
!242 = !DILocation(line: 64, column: 9, scope: !173)
!243 = !DILocation(line: 64, column: 9, scope: !174)
!244 = !DILocation(line: 65, column: 9, scope: !174)
!245 = !DILocation(line: 62, column: 18, scope: !176)
!246 = !DILocation(line: 68, column: 8, scope: !247)
!247 = distinct !DILexicalBlock(scope: !176, file: !3, line: 68, column: 8)
!248 = !{!187, !187, i64 0}
!249 = !DILocation(line: 68, column: 8, scope: !176)
!250 = !DILocation(line: 73, column: 5, scope: !251)
!251 = distinct !DILexicalBlock(scope: !252, file: !3, line: 71, column: 37)
!252 = distinct !DILexicalBlock(scope: !247, file: !3, line: 71, column: 13)
!253 = !{!235, !187, i64 4}
!254 = !DILocation(line: 75, column: 8, scope: !255)
!255 = distinct !DILexicalBlock(scope: !176, file: !3, line: 75, column: 8)
!256 = !DILocation(line: 75, column: 8, scope: !176)
!257 = !DILocation(line: 76, column: 44, scope: !255)
!258 = !DILocation(line: 76, column: 16, scope: !255)
!259 = !DILocation(line: 76, column: 9, scope: !255)
!260 = !DILocation(line: 78, column: 23, scope: !261)
!261 = distinct !DILexicalBlock(scope: !177, file: !3, line: 78, column: 13)
!262 = !DILocation(line: 78, column: 13, scope: !177)
!263 = !DILocation(line: 80, column: 13, scope: !264)
!264 = distinct !DILexicalBlock(scope: !265, file: !3, line: 80, column: 13)
!265 = distinct !DILexicalBlock(scope: !261, file: !3, line: 78, column: 29)
!266 = !DILocation(line: 80, column: 13, scope: !265)
!267 = !DILocation(line: 81, column: 48, scope: !264)
!268 = !DILocation(line: 81, column: 20, scope: !264)
!269 = !DILocation(line: 81, column: 13, scope: !264)
!270 = !DILocation(line: 85, column: 1, scope: !132)
