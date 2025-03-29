// Code generated by protoc-gen-go. DO NOT EDIT.
// versions:
<<<<<<< HEAD
// 	protoc-gen-go v1.36.3
=======
// 	protoc-gen-go v1.36.6
>>>>>>> 616ba7b (Add Android Support & Disable Alsa Backend)
// 	protoc        (unknown)
// source: spotify/lens/lens_model.proto

package lens

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

type Lens struct {
	state         protoimpl.MessageState `protogen:"open.v1"`
	Identifier    string                 `protobuf:"bytes,1,opt,name=identifier,proto3" json:"identifier,omitempty"`
	unknownFields protoimpl.UnknownFields
	sizeCache     protoimpl.SizeCache
}

func (x *Lens) Reset() {
	*x = Lens{}
	mi := &file_spotify_lens_lens_model_proto_msgTypes[0]
	ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
	ms.StoreMessageInfo(mi)
}

func (x *Lens) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*Lens) ProtoMessage() {}

func (x *Lens) ProtoReflect() protoreflect.Message {
	mi := &file_spotify_lens_lens_model_proto_msgTypes[0]
	if x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use Lens.ProtoReflect.Descriptor instead.
func (*Lens) Descriptor() ([]byte, []int) {
	return file_spotify_lens_lens_model_proto_rawDescGZIP(), []int{0}
}

func (x *Lens) GetIdentifier() string {
	if x != nil {
		return x.Identifier
	}
	return ""
}

type LensState struct {
	state         protoimpl.MessageState `protogen:"open.v1"`
	Identifier    string                 `protobuf:"bytes,1,opt,name=identifier,proto3" json:"identifier,omitempty"`
	Revision      []byte                 `protobuf:"bytes,2,opt,name=revision,proto3" json:"revision,omitempty"`
	unknownFields protoimpl.UnknownFields
	sizeCache     protoimpl.SizeCache
}

func (x *LensState) Reset() {
	*x = LensState{}
	mi := &file_spotify_lens_lens_model_proto_msgTypes[1]
	ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
	ms.StoreMessageInfo(mi)
}

func (x *LensState) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*LensState) ProtoMessage() {}

func (x *LensState) ProtoReflect() protoreflect.Message {
	mi := &file_spotify_lens_lens_model_proto_msgTypes[1]
	if x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use LensState.ProtoReflect.Descriptor instead.
func (*LensState) Descriptor() ([]byte, []int) {
	return file_spotify_lens_lens_model_proto_rawDescGZIP(), []int{1}
}

func (x *LensState) GetIdentifier() string {
	if x != nil {
		return x.Identifier
	}
	return ""
}

func (x *LensState) GetRevision() []byte {
	if x != nil {
		return x.Revision
	}
	return nil
}

var File_spotify_lens_lens_model_proto protoreflect.FileDescriptor

const file_spotify_lens_lens_model_proto_rawDesc = "" +
	"\n" +
	"\x1dspotify/lens/lens_model.proto\x12\x12spotify.lens.model\"&\n" +
	"\x04Lens\x12\x1e\n" +
	"\n" +
	"identifier\x18\x01 \x01(\tR\n" +
	"identifier\"G\n" +
	"\tLensState\x12\x1e\n" +
	"\n" +
	"identifier\x18\x01 \x01(\tR\n" +
	"identifier\x12\x1a\n" +
	"\brevision\x18\x02 \x01(\fR\brevisionB\xc8\x01\n" +
	"\x16com.spotify.lens.modelB\x0eLensModelProtoP\x01Z4github.com/devgianlu/go-librespot/proto/spotify/lens\xa2\x02\x03SLM\xaa\x02\x12Spotify.Lens.Model\xca\x02\x12Spotify\\Lens\\Model\xe2\x02\x1eSpotify\\Lens\\Model\\GPBMetadata\xea\x02\x14Spotify::Lens::Modelb\x06proto3"

var (
	file_spotify_lens_lens_model_proto_rawDescOnce sync.Once
	file_spotify_lens_lens_model_proto_rawDescData []byte
)

func file_spotify_lens_lens_model_proto_rawDescGZIP() []byte {
	file_spotify_lens_lens_model_proto_rawDescOnce.Do(func() {
		file_spotify_lens_lens_model_proto_rawDescData = protoimpl.X.CompressGZIP(unsafe.Slice(unsafe.StringData(file_spotify_lens_lens_model_proto_rawDesc), len(file_spotify_lens_lens_model_proto_rawDesc)))
	})
	return file_spotify_lens_lens_model_proto_rawDescData
}

var file_spotify_lens_lens_model_proto_msgTypes = make([]protoimpl.MessageInfo, 2)
var file_spotify_lens_lens_model_proto_goTypes = []any{
	(*Lens)(nil),      // 0: spotify.lens.model.Lens
	(*LensState)(nil), // 1: spotify.lens.model.LensState
}
var file_spotify_lens_lens_model_proto_depIdxs = []int32{
	0, // [0:0] is the sub-list for method output_type
	0, // [0:0] is the sub-list for method input_type
	0, // [0:0] is the sub-list for extension type_name
	0, // [0:0] is the sub-list for extension extendee
	0, // [0:0] is the sub-list for field type_name
}

func init() { file_spotify_lens_lens_model_proto_init() }
func file_spotify_lens_lens_model_proto_init() {
	if File_spotify_lens_lens_model_proto != nil {
		return
	}
	type x struct{}
	out := protoimpl.TypeBuilder{
		File: protoimpl.DescBuilder{
			GoPackagePath: reflect.TypeOf(x{}).PkgPath(),
			RawDescriptor: unsafe.Slice(unsafe.StringData(file_spotify_lens_lens_model_proto_rawDesc), len(file_spotify_lens_lens_model_proto_rawDesc)),
			NumEnums:      0,
			NumMessages:   2,
			NumExtensions: 0,
			NumServices:   0,
		},
		GoTypes:           file_spotify_lens_lens_model_proto_goTypes,
		DependencyIndexes: file_spotify_lens_lens_model_proto_depIdxs,
		MessageInfos:      file_spotify_lens_lens_model_proto_msgTypes,
	}.Build()
	File_spotify_lens_lens_model_proto = out.File
	file_spotify_lens_lens_model_proto_goTypes = nil
	file_spotify_lens_lens_model_proto_depIdxs = nil
}
