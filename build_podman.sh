TAG="ghcr.io/pspdev/pspdev:v20260201"

podman run \
	--rm -it \
	-v ./:/work_dir \
	-w /work_dir \
	$TAG \
	bash -c "
set -xe
cp procfw_sdk_prebuilt/include/* /usr/local/pspdev/psp/sdk/include/
cp procfw_sdk_prebuilt/lib/* /usr/local/pspdev/psp/sdk/lib/
make clean
make
"
