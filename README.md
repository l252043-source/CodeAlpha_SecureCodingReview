# 🔐 CodeAlpha_SecureCodingReview

<p align="center">
  <b>A manual secure code review of a C++ Restaurant Management System</b><br>
</p>

<p align="center">
  🛡️ Finding vulnerabilities before attackers do. 🛡️
</p>

---

## 📌 About This Project

This project is a **security audit** of *Dine 360*, a C++ OOP-based Restaurant Management System (built during my 2nd semester). The goal was to go beyond "does the code work?" and ask **"is the code safe?"** — reviewing authentication logic, data storage, and input handling for real-world security flaws.

The review identified **6 vulnerabilities**, ranging from Critical to Medium severity, each documented with its location, impact, and a concrete fix.

---

## 🔍 What Was Reviewed

- `AUTH.cpp` / `AUTH.h` — login & signup logic
- `USER.cpp` / `USER.h` — base user class
- `CUSTOMER.cpp` / `CUSTOMER.h` — customer accounts
- `STAFF.cpp` / `STAFF.h` — staff accounts & permissions
- `SIGNUPSCREEN.cpp` / `.h` — registration flow
- Data storage layer (`customer.txt`, `staff.txt`)

---

## 🚩 Key Findings

| # | Vulnerability | Severity |
|---|---|---|
| 1 | Passwords stored in **plaintext** (no hashing) | 🔴 Critical |
| 2 | **Hardcoded admin secret** in source code | 🔴 Critical |
| 3 | Unsanitized input causing **CSV/data corruption** | 🟠 High |
| 4 | Sensitive user data files unprotected & repo-tracked | 🟠 High |
| 5 | Password exposed via public getter method | 🟡 Medium |
| 6 | No login attempt limiting (brute-force risk) | 🟡 Medium |

📄 **Full details, impact analysis, and fixes:** see [`SECURITY_REVIEW.md`](./SECURITY_REVIEW.md)

---

## 🛠️ Example Fix: Password Hashing

The report includes a full before/after code example showing how plaintext password storage was converted to a **salted hash** implementation — see Section 3 of the report.

```cpp
// Before: raw password written directly to file
file << name << "," << username << "," << password << "," << address << endl;

// After: salted hash stored instead
string salt = generateSalt();
string hashed = hashPassword(password, salt);
file << name << "," << username << "," << hashed << "," << salt << "," << address << endl;
```

---

## 🛠️ Tools & Method

- **Manual code review** (line-by-line inspection)
- Pattern search across the codebase (`grep`) for hardcoded secrets, hashing usage, and unsafe function calls
- Root-cause + impact analysis for each finding

---

## 📚 What I Learned

This task shifted my perspective from "writing code that works" to "writing code that can't be trivially broken." I learned why plaintext password storage is one of the most dangerous — and most common — mistakes developers make, how hardcoded secrets in source code become public the moment a repo is shared, and how something as simple as an unescaped comma in a text file can become a data-integrity vulnerability. Most importantly, I practiced translating vulnerabilities into clear, actionable fixes — a core skill for any security-focused developer.

---

## 🎓 About CodeAlpha

This project was completed as part of the **CodeAlpha Cyber Security Virtual Internship**, which provides hands-on experience in network security, ethical hacking, and threat detection through real-world projects.

🔗 [www.codealpha.tech](https://www.codealpha.tech)

---

<p align="center">
  Made with 🔒 for the CodeAlpha Cyber Security Internship
</p>
