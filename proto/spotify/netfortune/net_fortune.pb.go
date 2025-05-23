// Code generated by protoc-gen-go. DO NOT EDIT.
// versions:
// 	protoc-gen-go v1.36.6
// 	protoc        (unknown)
// source: spotify/netfortune/net_fortune.proto

package netfortune

import (
	protoreflect "google.golang.org/protobuf/reflect/protoreflect"
	protoimpl "google.golang.org/protobuf/runtime/protoimpl"
	reflect "reflect"
	sync "sync"
	unsafe "unsafe"
)

const (
	// Verify that this generated code is sufficiently up-to-date.
	_ = protoimpl.EnforceVersion(20 - protoimpl.MinVersion)
	// Verify that runtime/protoimpl is sufficiently up-to-date.
	_ = protoimpl.EnforceVersion(protoimpl.MaxVersion - 20)
)

type NetFortuneResponse struct {
	state               protoimpl.MessageState `protogen:"open.v1"`
	AdvisedAudioBitrate int32                  `protobuf:"varint,1,opt,name=advised_audio_bitrate,json=advisedAudioBitrate,proto3" json:"advised_audio_bitrate,omitempty"`
	unknownFields       protoimpl.UnknownFields
	sizeCache           protoimpl.SizeCache
}

func (x *NetFortuneResponse) Reset() {
	*x = NetFortuneResponse{}
	mi := &file_spotify_netfortune_net_fortune_proto_msgTypes[0]
	ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
	ms.StoreMessageInfo(mi)
}

func (x *NetFortuneResponse) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*NetFortuneResponse) ProtoMessage() {}

func (x *NetFortuneResponse) ProtoReflect() protoreflect.Message {
	mi := &file_spotify_netfortune_net_fortune_proto_msgTypes[0]
	if x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use NetFortuneResponse.ProtoReflect.Descriptor instead.
func (*NetFortuneResponse) Descriptor() ([]byte, []int) {
	return file_spotify_netfortune_net_fortune_proto_rawDescGZIP(), []int{0}
}

func (x *NetFortuneResponse) GetAdvisedAudioBitrate() int32 {
	if x != nil {
		return x.AdvisedAudioBitrate
	}
	return 0
}

type NetFortuneV2Response struct {
	state               protoimpl.MessageState `protogen:"open.v1"`
	PredictId           string                 `protobuf:"bytes,1,opt,name=predict_id,json=predictId,proto3" json:"predict_id,omitempty"`
	EstimatedMaxBitrate int32                  `protobuf:"varint,2,opt,name=estimated_max_bitrate,json=estimatedMaxBitrate,proto3" json:"estimated_max_bitrate,omitempty"`
	// Types that are valid to be assigned to XAdvisedPrefetchBitrateMetered:
	//
	//	*NetFortuneV2Response_AdvisedPrefetchBitrateMetered
	XAdvisedPrefetchBitrateMetered isNetFortuneV2Response_XAdvisedPrefetchBitrateMetered `protobuf_oneof:"_advised_prefetch_bitrate_metered"`
	// Types that are valid to be assigned to XAdvisedPrefetchBitrateNonMetered:
	//
	//	*NetFortuneV2Response_AdvisedPrefetchBitrateNonMetered
	XAdvisedPrefetchBitrateNonMetered isNetFortuneV2Response_XAdvisedPrefetchBitrateNonMetered `protobuf_oneof:"_advised_prefetch_bitrate_non_metered"`
	unknownFields                     protoimpl.UnknownFields
	sizeCache                         protoimpl.SizeCache
}

func (x *NetFortuneV2Response) Reset() {
	*x = NetFortuneV2Response{}
	mi := &file_spotify_netfortune_net_fortune_proto_msgTypes[1]
	ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
	ms.StoreMessageInfo(mi)
}

func (x *NetFortuneV2Response) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*NetFortuneV2Response) ProtoMessage() {}

func (x *NetFortuneV2Response) ProtoReflect() protoreflect.Message {
	mi := &file_spotify_netfortune_net_fortune_proto_msgTypes[1]
	if x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use NetFortuneV2Response.ProtoReflect.Descriptor instead.
func (*NetFortuneV2Response) Descriptor() ([]byte, []int) {
	return file_spotify_netfortune_net_fortune_proto_rawDescGZIP(), []int{1}
}

func (x *NetFortuneV2Response) GetPredictId() string {
	if x != nil {
		return x.PredictId
	}
	return ""
}

func (x *NetFortuneV2Response) GetEstimatedMaxBitrate() int32 {
	if x != nil {
		return x.EstimatedMaxBitrate
	}
	return 0
}

func (x *NetFortuneV2Response) GetXAdvisedPrefetchBitrateMetered() isNetFortuneV2Response_XAdvisedPrefetchBitrateMetered {
	if x != nil {
		return x.XAdvisedPrefetchBitrateMetered
	}
	return nil
}

func (x *NetFortuneV2Response) GetAdvisedPrefetchBitrateMetered() int32 {
	if x != nil {
		if x, ok := x.XAdvisedPrefetchBitrateMetered.(*NetFortuneV2Response_AdvisedPrefetchBitrateMetered); ok {
			return x.AdvisedPrefetchBitrateMetered
		}
	}
	return 0
}

func (x *NetFortuneV2Response) GetXAdvisedPrefetchBitrateNonMetered() isNetFortuneV2Response_XAdvisedPrefetchBitrateNonMetered {
	if x != nil {
		return x.XAdvisedPrefetchBitrateNonMetered
	}
	return nil
}

func (x *NetFortuneV2Response) GetAdvisedPrefetchBitrateNonMetered() int32 {
	if x != nil {
		if x, ok := x.XAdvisedPrefetchBitrateNonMetered.(*NetFortuneV2Response_AdvisedPrefetchBitrateNonMetered); ok {
			return x.AdvisedPrefetchBitrateNonMetered
		}
	}
	return 0
}

type isNetFortuneV2Response_XAdvisedPrefetchBitrateMetered interface {
	isNetFortuneV2Response_XAdvisedPrefetchBitrateMetered()
}

type NetFortuneV2Response_AdvisedPrefetchBitrateMetered struct {
	AdvisedPrefetchBitrateMetered int32 `protobuf:"varint,3,opt,name=advised_prefetch_bitrate_metered,json=advisedPrefetchBitrateMetered,proto3,oneof"`
}

func (*NetFortuneV2Response_AdvisedPrefetchBitrateMetered) isNetFortuneV2Response_XAdvisedPrefetchBitrateMetered() {
}

type isNetFortuneV2Response_XAdvisedPrefetchBitrateNonMetered interface {
	isNetFortuneV2Response_XAdvisedPrefetchBitrateNonMetered()
}

type NetFortuneV2Response_AdvisedPrefetchBitrateNonMetered struct {
	AdvisedPrefetchBitrateNonMetered int32 `protobuf:"varint,4,opt,name=advised_prefetch_bitrate_non_metered,json=advisedPrefetchBitrateNonMetered,proto3,oneof"`
}

func (*NetFortuneV2Response_AdvisedPrefetchBitrateNonMetered) isNetFortuneV2Response_XAdvisedPrefetchBitrateNonMetered() {
}

var File_spotify_netfortune_net_fortune_proto protoreflect.FileDescriptor

const file_spotify_netfortune_net_fortune_proto_rawDesc = "" +
	"\n" +
	"$spotify/netfortune/net_fortune.proto\x12\x12spotify.netfortune\"H\n" +
	"\x12NetFortuneResponse\x122\n" +
	"\x15advised_audio_bitrate\x18\x01 \x01(\x05R\x13advisedAudioBitrate\"\xd4\x02\n" +
	"\x14NetFortuneV2Response\x12\x1d\n" +
	"\n" +
	"predict_id\x18\x01 \x01(\tR\tpredictId\x122\n" +
	"\x15estimated_max_bitrate\x18\x02 \x01(\x05R\x13estimatedMaxBitrate\x12I\n" +
	" advised_prefetch_bitrate_metered\x18\x03 \x01(\x05H\x00R\x1dadvisedPrefetchBitrateMetered\x12P\n" +
	"$advised_prefetch_bitrate_non_metered\x18\x04 \x01(\x05H\x01R advisedPrefetchBitrateNonMeteredB#\n" +
	"!_advised_prefetch_bitrate_meteredB'\n" +
	"%_advised_prefetch_bitrate_non_meteredB\xce\x01\n" +
	"\x16com.spotify.netfortuneB\x0fNetFortuneProtoP\x01Z:github.com/devgianlu/go-librespot/proto/spotify/netfortune\xa2\x02\x03SNX\xaa\x02\x12Spotify.Netfortune\xca\x02\x12Spotify\\Netfortune\xe2\x02\x1eSpotify\\Netfortune\\GPBMetadata\xea\x02\x13Spotify::Netfortuneb\x06proto3"

var (
	file_spotify_netfortune_net_fortune_proto_rawDescOnce sync.Once
	file_spotify_netfortune_net_fortune_proto_rawDescData []byte
)

func file_spotify_netfortune_net_fortune_proto_rawDescGZIP() []byte {
	file_spotify_netfortune_net_fortune_proto_rawDescOnce.Do(func() {
		file_spotify_netfortune_net_fortune_proto_rawDescData = protoimpl.X.CompressGZIP(unsafe.Slice(unsafe.StringData(file_spotify_netfortune_net_fortune_proto_rawDesc), len(file_spotify_netfortune_net_fortune_proto_rawDesc)))
	})
	return file_spotify_netfortune_net_fortune_proto_rawDescData
}

var file_spotify_netfortune_net_fortune_proto_msgTypes = make([]protoimpl.MessageInfo, 2)
var file_spotify_netfortune_net_fortune_proto_goTypes = []any{
	(*NetFortuneResponse)(nil),   // 0: spotify.netfortune.NetFortuneResponse
	(*NetFortuneV2Response)(nil), // 1: spotify.netfortune.NetFortuneV2Response
}
var file_spotify_netfortune_net_fortune_proto_depIdxs = []int32{
	0, // [0:0] is the sub-list for method output_type
	0, // [0:0] is the sub-list for method input_type
	0, // [0:0] is the sub-list for extension type_name
	0, // [0:0] is the sub-list for extension extendee
	0, // [0:0] is the sub-list for field type_name
}

func init() { file_spotify_netfortune_net_fortune_proto_init() }
func file_spotify_netfortune_net_fortune_proto_init() {
	if File_spotify_netfortune_net_fortune_proto != nil {
		return
	}
	file_spotify_netfortune_net_fortune_proto_msgTypes[1].OneofWrappers = []any{
		(*NetFortuneV2Response_AdvisedPrefetchBitrateMetered)(nil),
		(*NetFortuneV2Response_AdvisedPrefetchBitrateNonMetered)(nil),
	}
	type x struct{}
	out := protoimpl.TypeBuilder{
		File: protoimpl.DescBuilder{
			GoPackagePath: reflect.TypeOf(x{}).PkgPath(),
			RawDescriptor: unsafe.Slice(unsafe.StringData(file_spotify_netfortune_net_fortune_proto_rawDesc), len(file_spotify_netfortune_net_fortune_proto_rawDesc)),
			NumEnums:      0,
			NumMessages:   2,
			NumExtensions: 0,
			NumServices:   0,
		},
		GoTypes:           file_spotify_netfortune_net_fortune_proto_goTypes,
		DependencyIndexes: file_spotify_netfortune_net_fortune_proto_depIdxs,
		MessageInfos:      file_spotify_netfortune_net_fortune_proto_msgTypes,
	}.Build()
	File_spotify_netfortune_net_fortune_proto = out.File
	file_spotify_netfortune_net_fortune_proto_goTypes = nil
	file_spotify_netfortune_net_fortune_proto_depIdxs = nil
}
