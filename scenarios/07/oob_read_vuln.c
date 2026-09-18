/*
 * =============================================================================
 * Tên tệp       : oob_read_vuln.c
 * Kịch bản       : Out-of-bounds Read (CWE-125) - #3 CWE Top 25 (2023)
 * Mô tả         : Phiên bản CÓ LỖ HỔNG - đọc dữ liệu ngoài phạm vi mảng
 *
 * So với bản gốc, thay đổi:
 *   - Hàm doc_phan_tu() không kiểm tra biên -> đọc bộ nhớ ngoài mảng
 *   - Có thể làm lộ thông tin nhạy cảm (key, password, flag bảo mật)
 *
 * Lỗ hổng:
 *   - CWE-125: Out-of-bounds Read
 *   - CWE-200: Information Exposure
 *   - CVE liên quan: CVE-2014-0160 (Heartbleed - OpenSSL), CVE-2020-0796 (SMBGhost)
 *   - Rủi ro: lộ khóa bí mật, mật khẩu, dữ liệu nhạy cảm trong bộ nhớ
 *
 * Dùng trong demo: SCGL phải phát hiện commit này là BUGGY.
 * =============================================================================
 */

#include <stdio.h>
#include <string.h>

#define KICH_THUOC_MANG    10

/* Mô phỏng bộ nhớ chứa dữ liệu nhạy cảm (key, mật khẩu) */
static const char BI_MAT_NHAY_CAM[] = "SECRET_KEY=ABC123XYZ_super_secret_password";

int khoi_tao_mang(int *mang, int kich_thuoc) {
    if (mang == NULL || kich_thuoc <= 0) return -1;
    for (int i = 0; i < kich_thuoc; i++) {
        mang[i] = i * 10;
    }
    return 0;
}

/* HÀM NÀY CHỨA LỖ HỔNG OUT-OF-BOUNDS READ */
int doc_phan_tu(const int *mang, int vi_tri) {
    if (mang == NULL) return -1;
    /* LỖI: Không kiểm tra vi_tri có nằm trong mảng không! */
    /* Có thể đọc ra ngoài mảng -> lộ dữ liệu nhạy cảm */
    return mang[vi_tri];
}

/* Mô phỏng: ghi dữ liệu nhạy cảm ngay sau mảng */
void* mo_phong_bo_nho_nhay_cam(int *mang, int kich_thuoc) {
    /* Tạo vùng nhớ liên kề với mảng chứa "bí mật" */
    void *buf = malloc(kich_thuoc * sizeof(int) + sizeof(BI_MAT_NHAY_CAM));
    if (buf == NULL) return NULL;
    /* Phần đầu là mảng int */
    int *int_ptr = (int *)buf;
    for (int i = 0; i < kich_thuoc; i++) int_ptr[i] = i * 10;
    /* Phần sau là dữ liệu nhạy cảm */
    memcpy((char *)buf + kich_thuoc * sizeof(int),
           BI_MAT_NHAY_CAM, sizeof(BI_MAT_NHAY_CAM));
    /* Trỏ mảng của caller vào phần đầu */
    return buf;
}

int main(int argc, char **argv) {
    void *buf = mo_phong_bo_nho_nhay_cam(NULL, KICH_THUOC_MANG);
    if (buf == NULL) return 1;
    int *mang = (int *)buf;

    khoi_tao_mang(mang, KICH_THUOC_MANG);

    /* Đọc hợp lệ */
    printf("[Hop le] mang[3] = %d\n", doc_phan_tu(mang, 3));

    /* ĐỌC NGOÀI PHẠM VI - lộ dữ liệu nhạy cảm */
    int gia_tri = doc_phan_tu(mang, KICH_THUOC_MANG + 1);
    printf("[CANH BAO] OOB Read: doc duoc gia tri ngoai pham vi = %d\n", gia_tri);
    printf("[CANH BAO] Co the doc duoc du lieu nhay cam!\n");

    free(buf);
    return 0;
}
