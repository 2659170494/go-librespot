// Code generated by protoc-gen-go. DO NOT EDIT.
// versions:
// 	protoc-gen-go v1.36.6
// 	protoc        (unknown)
// source: spotify/login5/v3/login5_user_info.proto

package login5v3

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

type UserInfo_Gender int32

const (
	UserInfo_UNKNOWN UserInfo_Gender = 0
	UserInfo_MALE    UserInfo_Gender = 1
	UserInfo_FEMALE  UserInfo_Gender = 2
	UserInfo_NEUTRAL UserInfo_Gender = 3
)

// Enum value maps for UserInfo_Gender.
var (
	UserInfo_Gender_name = map[int32]string{
		0: "UNKNOWN",
		1: "MALE",
		2: "FEMALE",
		3: "NEUTRAL",
	}
	UserInfo_Gender_value = map[string]int32{
		"UNKNOWN": 0,
		"MALE":    1,
		"FEMALE":  2,
		"NEUTRAL": 3,
	}
)

func (x UserInfo_Gender) Enum() *UserInfo_Gender {
	p := new(UserInfo_Gender)
	*p = x
	return p
}

func (x UserInfo_Gender) String() string {
	return protoimpl.X.EnumStringOf(x.Descriptor(), protoreflect.EnumNumber(x))
}

func (UserInfo_Gender) Descriptor() protoreflect.EnumDescriptor {
	return file_spotify_login5_v3_login5_user_info_proto_enumTypes[0].Descriptor()
}

func (UserInfo_Gender) Type() protoreflect.EnumType {
	return &file_spotify_login5_v3_login5_user_info_proto_enumTypes[0]
}

func (x UserInfo_Gender) Number() protoreflect.EnumNumber {
	return protoreflect.EnumNumber(x)
}

// Deprecated: Use UserInfo_Gender.Descriptor instead.
func (UserInfo_Gender) EnumDescriptor() ([]byte, []int) {
	return file_spotify_login5_v3_login5_user_info_proto_rawDescGZIP(), []int{0, 0}
}

type UserInfo struct {
	state                  protoimpl.MessageState `protogen:"open.v1"`
	Name                   string                 `protobuf:"bytes,1,opt,name=name,proto3" json:"name,omitempty"`
	Email                  string                 `protobuf:"bytes,2,opt,name=email,proto3" json:"email,omitempty"`
	EmailVerified          bool                   `protobuf:"varint,3,opt,name=email_verified,json=emailVerified,proto3" json:"email_verified,omitempty"`
	Birthdate              string                 `protobuf:"bytes,4,opt,name=birthdate,proto3" json:"birthdate,omitempty"`
	Gender                 UserInfo_Gender        `protobuf:"varint,5,opt,name=gender,proto3,enum=spotify.login5.v3.UserInfo_Gender" json:"gender,omitempty"`
	PhoneNumber            string                 `protobuf:"bytes,6,opt,name=phone_number,json=phoneNumber,proto3" json:"phone_number,omitempty"`
	PhoneNumberVerified    bool                   `protobuf:"varint,7,opt,name=phone_number_verified,json=phoneNumberVerified,proto3" json:"phone_number_verified,omitempty"`
	EmailAlreadyRegistered bool                   `protobuf:"varint,8,opt,name=email_already_registered,json=emailAlreadyRegistered,proto3" json:"email_already_registered,omitempty"`
	unknownFields          protoimpl.UnknownFields
	sizeCache              protoimpl.SizeCache
}

func (x *UserInfo) Reset() {
	*x = UserInfo{}
	mi := &file_spotify_login5_v3_login5_user_info_proto_msgTypes[0]
	ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
	ms.StoreMessageInfo(mi)
}

func (x *UserInfo) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*UserInfo) ProtoMessage() {}

func (x *UserInfo) ProtoReflect() protoreflect.Message {
	mi := &file_spotify_login5_v3_login5_user_info_proto_msgTypes[0]
	if x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use UserInfo.ProtoReflect.Descriptor instead.
func (*UserInfo) Descriptor() ([]byte, []int) {
	return file_spotify_login5_v3_login5_user_info_proto_rawDescGZIP(), []int{0}
}

func (x *UserInfo) GetName() string {
	if x != nil {
		return x.Name
	}
	return ""
}

func (x *UserInfo) GetEmail() string {
	if x != nil {
		return x.Email
	}
	return ""
}

func (x *UserInfo) GetEmailVerified() bool {
	if x != nil {
		return x.EmailVerified
	}
	return false
}

func (x *UserInfo) GetBirthdate() string {
	if x != nil {
		return x.Birthdate
	}
	return ""
}

func (x *UserInfo) GetGender() UserInfo_Gender {
	if x != nil {
		return x.Gender
	}
	return UserInfo_UNKNOWN
}

func (x *UserInfo) GetPhoneNumber() string {
	if x != nil {
		return x.PhoneNumber
	}
	return ""
}

func (x *UserInfo) GetPhoneNumberVerified() bool {
	if x != nil {
		return x.PhoneNumberVerified
	}
	return false
}

func (x *UserInfo) GetEmailAlreadyRegistered() bool {
	if x != nil {
		return x.EmailAlreadyRegistered
	}
	return false
}

var File_spotify_login5_v3_login5_user_info_proto protoreflect.FileDescriptor

const file_spotify_login5_v3_login5_user_info_proto_rawDesc = "" +
	"\n" +
	"(spotify/login5/v3/login5_user_info.proto\x12\x11spotify.login5.v3\"\x80\x03\n" +
	"\bUserInfo\x12\x12\n" +
	"\x04name\x18\x01 \x01(\tR\x04name\x12\x14\n" +
	"\x05email\x18\x02 \x01(\tR\x05email\x12%\n" +
	"\x0eemail_verified\x18\x03 \x01(\bR\remailVerified\x12\x1c\n" +
	"\tbirthdate\x18\x04 \x01(\tR\tbirthdate\x12:\n" +
	"\x06gender\x18\x05 \x01(\x0e2\".spotify.login5.v3.UserInfo.GenderR\x06gender\x12!\n" +
	"\fphone_number\x18\x06 \x01(\tR\vphoneNumber\x122\n" +
	"\x15phone_number_verified\x18\a \x01(\bR\x13phoneNumberVerified\x128\n" +
	"\x18email_already_registered\x18\b \x01(\bR\x16emailAlreadyRegistered\"8\n" +
	"\x06Gender\x12\v\n" +
	"\aUNKNOWN\x10\x00\x12\b\n" +
	"\x04MALE\x10\x01\x12\n" +
	"\n" +
	"\x06FEMALE\x10\x02\x12\v\n" +
	"\aNEUTRAL\x10\x03B\xd6\x01\n" +
	"\x15com.spotify.login5.v3B\x13Login5UserInfoProtoP\x01ZBgithub.com/devgianlu/go-librespot/proto/spotify/login5/v3;login5v3\xa2\x02\x03SLX\xaa\x02\x11Spotify.Login5.V3\xca\x02\x11Spotify\\Login5\\V3\xe2\x02\x1dSpotify\\Login5\\V3\\GPBMetadata\xea\x02\x13Spotify::Login5::V3b\x06proto3"

var (
	file_spotify_login5_v3_login5_user_info_proto_rawDescOnce sync.Once
	file_spotify_login5_v3_login5_user_info_proto_rawDescData []byte
)

func file_spotify_login5_v3_login5_user_info_proto_rawDescGZIP() []byte {
	file_spotify_login5_v3_login5_user_info_proto_rawDescOnce.Do(func() {
		file_spotify_login5_v3_login5_user_info_proto_rawDescData = protoimpl.X.CompressGZIP(unsafe.Slice(unsafe.StringData(file_spotify_login5_v3_login5_user_info_proto_rawDesc), len(file_spotify_login5_v3_login5_user_info_proto_rawDesc)))
	})
	return file_spotify_login5_v3_login5_user_info_proto_rawDescData
}

var file_spotify_login5_v3_login5_user_info_proto_enumTypes = make([]protoimpl.EnumInfo, 1)
var file_spotify_login5_v3_login5_user_info_proto_msgTypes = make([]protoimpl.MessageInfo, 1)
var file_spotify_login5_v3_login5_user_info_proto_goTypes = []any{
	(UserInfo_Gender)(0), // 0: spotify.login5.v3.UserInfo.Gender
	(*UserInfo)(nil),     // 1: spotify.login5.v3.UserInfo
}
var file_spotify_login5_v3_login5_user_info_proto_depIdxs = []int32{
	0, // 0: spotify.login5.v3.UserInfo.gender:type_name -> spotify.login5.v3.UserInfo.Gender
	1, // [1:1] is the sub-list for method output_type
	1, // [1:1] is the sub-list for method input_type
	1, // [1:1] is the sub-list for extension type_name
	1, // [1:1] is the sub-list for extension extendee
	0, // [0:1] is the sub-list for field type_name
}

func init() { file_spotify_login5_v3_login5_user_info_proto_init() }
func file_spotify_login5_v3_login5_user_info_proto_init() {
	if File_spotify_login5_v3_login5_user_info_proto != nil {
		return
	}
	type x struct{}
	out := protoimpl.TypeBuilder{
		File: protoimpl.DescBuilder{
			GoPackagePath: reflect.TypeOf(x{}).PkgPath(),
			RawDescriptor: unsafe.Slice(unsafe.StringData(file_spotify_login5_v3_login5_user_info_proto_rawDesc), len(file_spotify_login5_v3_login5_user_info_proto_rawDesc)),
			NumEnums:      1,
			NumMessages:   1,
			NumExtensions: 0,
			NumServices:   0,
		},
		GoTypes:           file_spotify_login5_v3_login5_user_info_proto_goTypes,
		DependencyIndexes: file_spotify_login5_v3_login5_user_info_proto_depIdxs,
		EnumInfos:         file_spotify_login5_v3_login5_user_info_proto_enumTypes,
		MessageInfos:      file_spotify_login5_v3_login5_user_info_proto_msgTypes,
	}.Build()
	File_spotify_login5_v3_login5_user_info_proto = out.File
	file_spotify_login5_v3_login5_user_info_proto_goTypes = nil
	file_spotify_login5_v3_login5_user_info_proto_depIdxs = nil
}
