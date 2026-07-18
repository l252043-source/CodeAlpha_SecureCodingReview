# Secure Coding Review — Dine 360 (Restaurant Management System)

**CodeAlpha Cyber Security Internship — Task 3**

**Project Reviewed:** Dine 360 – Restaurant Management System
**Language:** C++ (Object-Oriented)
**Review Type:** Manual source code inspection focused on authentication, data storage, and input handling
**Files Reviewed:** `AUTH.cpp/.h`, `USER.cpp/.h`, `CUSTOMER.cpp/.h`, `STAFF.cpp/.h`, `SIGNUPSCREEN.cpp/.h`, `LOGINSCREEN.cpp/.h`, data files (`customer.txt`, `staff.txt`)

---

## 1. Executive Summary

Dine 360 is a C++ restaurant management system with customer and staff roles, login/signup screens, ordering, and reservation features. The review focused on the authentication and data-storage layer, since that is where the most security-sensitive operations happen (handling usernames, passwords, and role-based access).

**6 vulnerabilities** were identified, ranging from **Critical** to **Medium** severity. The most serious issues are the complete absence of password hashing and a hardcoded administrator secret embedded directly in the source code.

| Severity | Count |
|---|---|
| 🔴 Critical | 2 |
| 🟠 High | 2 |
| 🟡 Medium | 2 |

---

## 2. Findings

### 🔴 Finding 1: Passwords Stored in Plaintext
**Location:** `AUTH.cpp` — `writeCustomer()`, `writeStaff()`, `login()`

```cpp
void Auth::writeCustomer(string name, string username, string password, string address) {
    ofstream file("customer.txt", ios::app);
    file << name << "," << username << "," << password << "," << address << "," << endl;
}
```

Passwords are written to `customer.txt` / `staff.txt` exactly as the user typed them, with no hashing. Login simply compares the entered password to the stored plaintext value (`if (u == username && p == password)`).

**Impact:** Anyone who can open these text files (a classmate, a grader, anyone with access to the machine, or anyone who accidentally uploads the repo publicly) can read every user's real password instantly. Because people frequently reuse passwords, this can compromise their accounts on other services too.

**Evidence:** The current data files in the project already contain readable passwords such as `Critical_69` and `Hdr_12`.

**Recommendation:** Never store raw passwords. Hash every password with a slow, salted hashing algorithm before writing it to disk, and compare hashes at login instead of raw strings. See the secure code example in Section 3.

---

### 🔴 Finding 2: Hardcoded Administrator Secret
**Location:** `AUTH.cpp` line 58 and `SIGNUPSCREEN.cpp` line 119

```cpp
if (code != "DINE360ADMIN") {
    return false;
}
```

The "admin code" required to register as staff is a fixed string compiled directly into the executable, duplicated in two files.

**Impact:** Anyone who downloads/decompiles the `.exe`, or simply reads the public GitHub repository, can extract this string and register themselves as a manager with full permissions (`"all"`). This defeats the entire staff-access-control system.

**Recommendation:** Remove the hardcoded string entirely. Real-world alternatives:
- Require an existing manager to approve/create new staff accounts from within the app.
- If a shared setup code is unavoidable, store it outside source control (e.g., a local config file excluded via `.gitignore`) and rotate it periodically — never commit it to Git history.

---

### 🟠 Finding 3: Unsanitized Input Breaks Data Integrity (CSV Injection)
**Location:** `AUTH.cpp` — all `write*()` and `read*()` functions

Data is stored as comma-separated values with no escaping:
```cpp
file << name << "," << username << "," << password << "," << address << "," << endl;
```

**Impact:** This is not just a bug — it's a security-relevant integrity flaw. If a user enters a comma anywhere (e.g., an address like `"735, Lahore"`), it silently shifts every field after it, corrupting the record. This is directly visible in the project's own `customer.txt`, where one row already has a broken field count. A malicious user could deliberately craft input containing commas to try to inject extra fields (for example, attempting to append a fake permission field for a staff record).

**Recommendation:**
- Reject or strip delimiter characters (`,`) from user input before saving, **or**
- Escape/quote fields properly (like real CSV writers do), **or**
- Switch to a safer storage format such as JSON, which handles special characters correctly by design.

---

### 🟠 Finding 4: Sensitive Data Files Are Unprotected and Repo-Committed
**Location:** `customer.txt`, `staff.txt` (project root)

**Impact:** These files sit in plain sight in the project folder with no access restriction, and — based on the uploaded project — appear to be tracked in the repository itself, meaning they'd be pushed to GitHub along with the code.

**Recommendation:**
- Add `customer.txt`, `staff.txt`, and any other generated data files to `.gitignore` so they are never committed.
- Before pushing this repo publicly, remove the existing data files from Git history (`git rm --cached` + commit), since Git retains old commits even after a file is deleted.
- At minimum, replace the current files with fake/sample data before making the repository public.

---

### 🟡 Finding 5: Password Exposed via Public Getter
**Location:** `USER.h` / `USER.cpp`

```cpp
string getPassword() const;
```

**Impact:** Any part of the codebase (or future code) that holds a `User`, `Customer`, or `Staff` object can retrieve the plaintext password at any time. This widens the "attack surface" — the more places a secret can be accessed from, the more places it can leak from (accidentally logged, displayed, copied, etc.).

**Recommendation:** Once passwords are hashed (Finding 1), there's rarely a legitimate reason to read a password back out. Remove `getPassword()` where possible, or restrict it to only the internal authentication logic that verifies hashes.

---

### 🟡 Finding 6: No Login Attempt Limiting
**Location:** `AUTH.cpp` — `login()`

**Impact:** The login function will accept unlimited attempts with no delay or lockout. Combined with Finding 1 (plaintext passwords, meaning any leak is instantly usable) and simple passwords, this makes brute-force guessing straightforward, especially since the underlying data is just a local text file that could also be brute-forced offline.

**Recommendation:** Add a basic attempt counter per username with a short lockout/delay after repeated failures (e.g., 5 attempts → 30-second cooldown).

---

## 3. Secure Code Example (Before vs. After)

The example below demonstrates fixing **Finding 1** (plaintext passwords) using salted SHA-256 hashing as a minimum viable improvement. *(For a production system, a purpose-built password-hashing algorithm like bcrypt or Argon2 is preferred over raw SHA-256, but this example keeps it achievable in plain C++ without external crypto libraries.)*

**❌ Before (vulnerable):**
```cpp
void Auth::writeCustomer(string name, string username, string password, string address) {
    ofstream file("customer.txt", ios::app);
    file << name << "," << username << "," << password << "," << address << "," << endl;
}

// Login check:
if (u == username && p == password) { ... }
```

**✅ After (hashed + salted):**
```cpp
#include <functional>
#include <sstream>
#include <random>

// Generates a random salt per user
string generateSalt() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 61);
    const string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    string salt;
    for (int i = 0; i < 16; i++) salt += chars[dist(gen)];
    return salt;
}

// Combines password + salt and hashes them
string hashPassword(const string& password, const string& salt) {
    std::hash<string> hasher;
    size_t hashValue = hasher(password + salt);
    std::stringstream ss;
    ss << std::hex << hashValue;
    return ss.str();
}

void Auth::writeCustomer(string name, string username, string password, string address) {
    string salt = generateSalt();
    string hashed = hashPassword(password, salt);

    ofstream file("customer.txt", ios::app);
    // Store salt alongside the hash — never the raw password
    file << name << "," << username << "," << hashed << "," << salt << "," << address << endl;
}

// Login check — compare hashes, never raw passwords:
string hashedAttempt = hashPassword(enteredPassword, storedSalt);
if (u == username && hashedAttempt == storedHash) { ... }
```

> **Note:** `std::hash` is used here for simplicity and portability without extra dependencies. For real-world / production use, integrate a proper library such as **OpenSSL (SHA-256)**, **libsodium**, or **bcrypt** for stronger, industry-standard password hashing.

---

## 4. Summary of Recommendations

| Priority | Action |
|---|---|
| 1 | Hash + salt all passwords before storing; compare hashes at login |
| 2 | Remove the hardcoded `"DINE360ADMIN"` secret; use an in-app approval flow instead |
| 3 | Sanitize or escape user input to prevent comma/delimiter collisions |
| 4 | Add data files to `.gitignore`; scrub them from Git history before making the repo public |
| 5 | Remove/restrict `getPassword()` |
| 6 | Add login attempt limiting to slow down brute-force attempts |

---

## 5. Conclusion

Dine 360 is a solid, functional OOP project with a clean class structure (`User` → `Customer` / `Staff` inheritance, separated screens, and managers). However, like many student projects, it prioritized functionality over security — which is exactly what this review process is meant to catch before code reaches a real production environment. Implementing the fixes above, especially password hashing and removing the hardcoded admin code, would bring the authentication system up to a baseline acceptable for real-world deployment.

---

*This review was conducted as part of the CodeAlpha Cyber Security Internship — Task 3: Secure Coding Review.*
