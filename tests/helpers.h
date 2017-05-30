#pragma once

static void assert(bool condition, const wchar_t *msg = nullptr) {
    Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue(condition, msg);
}

static void fail(const wchar_t *msg = nullptr) {
    Microsoft::VisualStudio::CppUnitTestFramework::Assert::Fail(msg);
}

static void pass() { }
