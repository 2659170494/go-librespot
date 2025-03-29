// Code generated by protoc-gen-go. DO NOT EDIT.
// versions:
<<<<<<< HEAD
// 	protoc-gen-go v1.36.3
=======
// 	protoc-gen-go v1.36.6
>>>>>>> 616ba7b (Add Android Support & Disable Alsa Backend)
// 	protoc        (unknown)
// source: spotify/playlist/signal/signal_model.proto

package signal

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

type Signal struct {
	state         protoimpl.MessageState `protogen:"open.v1"`
	Identifier    string                 `protobuf:"bytes,1,opt,name=identifier,proto3" json:"identifier,omitempty"`
	Data          []byte                 `protobuf:"bytes,2,opt,name=data,proto3" json:"data,omitempty"`
	ClientPayload []byte                 `protobuf:"bytes,3,opt,name=client_payload,json=clientPayload,proto3" json:"client_payload,omitempty"`
	unknownFields protoimpl.UnknownFields
	sizeCache     protoimpl.SizeCache
}

func (x *Signal) Reset() {
	*x = Signal{}
	mi := &file_spotify_playlist_signal_signal_model_proto_msgTypes[0]
	ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
	ms.StoreMessageInfo(mi)
}

func (x *Signal) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*Signal) ProtoMessage() {}

func (x *Signal) ProtoReflect() protoreflect.Message {
	mi := &file_spotify_playlist_signal_signal_model_proto_msgTypes[0]
	if x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use Signal.ProtoReflect.Descriptor instead.
func (*Signal) Descriptor() ([]byte, []int) {
	return file_spotify_playlist_signal_signal_model_proto_rawDescGZIP(), []int{0}
}

func (x *Signal) GetIdentifier() string {
	if x != nil {
		return x.Identifier
	}
	return ""
}

func (x *Signal) GetData() []byte {
	if x != nil {
		return x.Data
	}
	return nil
}

func (x *Signal) GetClientPayload() []byte {
	if x != nil {
		return x.ClientPayload
	}
	return nil
}

var File_spotify_playlist_signal_signal_model_proto protoreflect.FileDescriptor

const file_spotify_playlist_signal_signal_model_proto_rawDesc = "" +
	"\n" +
	"*spotify/playlist/signal/signal_model.proto\x12\x17spotify.playlist.signal\"c\n" +
	"\x06Signal\x12\x1e\n" +
	"\n" +
	"identifier\x18\x01 \x01(\tR\n" +
	"identifier\x12\x12\n" +
	"\x04data\x18\x02 \x01(\fR\x04data\x12%\n" +
	"\x0eclient_payload\x18\x03 \x01(\fR\rclientPayloadB\xee\x01\n" +
	"\x1bcom.spotify.playlist.signalB\x10SignalModelProtoP\x01Z?github.com/devgianlu/go-librespot/proto/spotify/playlist/signal\xa2\x02\x03SPS\xaa\x02\x17Spotify.Playlist.Signal\xca\x02\x17Spotify\\Playlist\\Signal\xe2\x02#Spotify\\Playlist\\Signal\\GPBMetadata\xea\x02\x19Spotify::Playlist::Signalb\x06proto3"

var (
	file_spotify_playlist_signal_signal_model_proto_rawDescOnce sync.Once
	file_spotify_playlist_signal_signal_model_proto_rawDescData []byte
)

func file_spotify_playlist_signal_signal_model_proto_rawDescGZIP() []byte {
	file_spotify_playlist_signal_signal_model_proto_rawDescOnce.Do(func() {
		file_spotify_playlist_signal_signal_model_proto_rawDescData = protoimpl.X.CompressGZIP(unsafe.Slice(unsafe.StringData(file_spotify_playlist_signal_signal_model_proto_rawDesc), len(file_spotify_playlist_signal_signal_model_proto_rawDesc)))
	})
	return file_spotify_playlist_signal_signal_model_proto_rawDescData
}

var file_spotify_playlist_signal_signal_model_proto_msgTypes = make([]protoimpl.MessageInfo, 1)
var file_spotify_playlist_signal_signal_model_proto_goTypes = []any{
	(*Signal)(nil), // 0: spotify.playlist.signal.Signal
}
var file_spotify_playlist_signal_signal_model_proto_depIdxs = []int32{
	0, // [0:0] is the sub-list for method output_type
	0, // [0:0] is the sub-list for method input_type
	0, // [0:0] is the sub-list for extension type_name
	0, // [0:0] is the sub-list for extension extendee
	0, // [0:0] is the sub-list for field type_name
}

func init() { file_spotify_playlist_signal_signal_model_proto_init() }
func file_spotify_playlist_signal_signal_model_proto_init() {
	if File_spotify_playlist_signal_signal_model_proto != nil {
		return
	}
	type x struct{}
	out := protoimpl.TypeBuilder{
		File: protoimpl.DescBuilder{
			GoPackagePath: reflect.TypeOf(x{}).PkgPath(),
			RawDescriptor: unsafe.Slice(unsafe.StringData(file_spotify_playlist_signal_signal_model_proto_rawDesc), len(file_spotify_playlist_signal_signal_model_proto_rawDesc)),
			NumEnums:      0,
			NumMessages:   1,
			NumExtensions: 0,
			NumServices:   0,
		},
		GoTypes:           file_spotify_playlist_signal_signal_model_proto_goTypes,
		DependencyIndexes: file_spotify_playlist_signal_signal_model_proto_depIdxs,
		MessageInfos:      file_spotify_playlist_signal_signal_model_proto_msgTypes,
	}.Build()
	File_spotify_playlist_signal_signal_model_proto = out.File
	file_spotify_playlist_signal_signal_model_proto_goTypes = nil
	file_spotify_playlist_signal_signal_model_proto_depIdxs = nil
}
