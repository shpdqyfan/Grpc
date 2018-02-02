# Grpc

1.The following commands used to generate source files from gnmi.proto.
protoc --cpp_out=./ gnmi.proto
protoc --grpc_out=./ --plugin=protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin gnmi.proto
