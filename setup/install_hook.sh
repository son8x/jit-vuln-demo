#!/bin/bash
# ============================================================
#  install_hook.sh - Cai dat pre-commit hook vao repo GitHub
# ============================================================
# Tuong tu install_hook.bat nhung danh cho Linux/Mac
# ============================================================

set -e

echo "============================================================"
echo "  CAI DAT PRE-COMMIT HOOK"
echo "============================================================"
echo

# Kiem tra git repo
if ! git rev-parse --show-toplevel >/dev/null 2>&1; then
    echo "[LOI] Thu muc hien tai khong phai git repository!"
    exit 1
fi

REPO_ROOT=$(git rev-parse --show-toplevel)
echo "[INFO] Repo root: $REPO_ROOT"
echo

# Tim SCGL model
SCGL_DIR=""
DEMO_DIR=""

# Thu cac vi tri thuong gap
for candidate in "../../code/SCGL" "../code/SCGL" "../../../code/SCGL"; do
    if [ -f "${candidate}/.venv/Scripts/python.exe" ] || [ -f "${candidate}/.venv/bin/python" ]; then
        SCGL_DIR="$candidate"
        DEMO_DIR="$(dirname $candidate)/demo"
        break
    fi
done

if [ -z "$SCGL_DIR" ]; then
    echo "[CANH BAO] Khong tu dong tim thay SCGL."
    read -p "Nhap duong dan toi SCGL: " SCGL_DIR
    DEMO_DIR="$(cd "$(dirname "$SCGL_DIR")/demo" 2>/dev/null && pwd || echo "")"
fi

SCGL_DIR_FULL=$(cd "$SCGL_DIR" && pwd)
DEMO_DIR_FULL=$(cd "$DEMO_DIR" && pwd)

echo "[INFO] SCGL dir: $SCGL_DIR_FULL"
echo "[INFO] Demo dir: $DEMO_DIR_FULL"
echo

# Copy hook
HOOK_SRC="$DEMO_DIR_FULL/server/git_hook_template.sh"
HOOK_DST="$REPO_ROOT/.git/hooks/pre-commit"

if [ ! -f "$HOOK_SRC" ]; then
    echo "[LOI] Khong tim thay hook: $HOOK_SRC"
    exit 1
fi

echo "[1/4] Copy hook..."
cp "$HOOK_SRC" "$HOOK_DST"
chmod +x "$HOOK_DST"
echo "      OK"
echo

# Tao .env
ENV_FILE="$REPO_ROOT/.env"
echo "[2/4] Tao .env..."
cat > "$ENV_FILE" << EOF
# Cau hinh cho pre-commit hook
JIT_VULN_DEMO_DIR=$DEMO_DIR_FULL
JIT_VULN_SCGL_DIR=$SCGL_DIR_FULL
JIT_VULN_PYTHON=$SCGL_DIR_FULL/.venv/bin/python
EOF
echo "      Da tao: $ENV_FILE"
echo

# .gitignore
GITIGNORE="$REPO_ROOT/.gitignore"
echo "[3/4] Cap nhat .gitignore..."
touch "$GITIGNORE"
if ! grep -q "^\.env$" "$GITIGNORE"; then
    echo "" >> "$GITIGNORE"
    echo ".env" >> "$GITIGNORE"
    echo "      Da them .env"
fi
echo

# Git user
echo "[4/4] Cau hinh git user..."
if [ -z "$(git config user.name)" ]; then
    read -p "Ten hien thi: " GIT_NAME
    git config user.name "$GIT_NAME"
fi
if [ -z "$(git config user.email)" ]; then
    read -p "Email GitHub: " GIT_EMAIL
    git config user.email "$GIT_EMAIL"
fi
echo

echo "============================================================"
echo "  HOAN THANH!"
echo "============================================================"
echo
echo "Buoc tiep theo:"
echo "  git add ."
echo "  git commit -m 'init: add demo files'"
echo "  git push origin main"
echo "  python auto_demo.py"
