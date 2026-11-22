#include "TestCommon.hpp"
#include "helpers/RaiiHandles.hpp"
#include <cerrno>
#include <fcntl.h>
#include <iostream>
#include <utility>

namespace {

bool sfb_IsFdOpen(int i32_Fd) {
    return -1 != ::fcntl(i32_Fd, F_GETFD) || EBADF != errno;
}

} // namespace

void fv_RunTestRaiiHandles() {
    // 1. CUniqueFd default construction, scope release, and fv_Reset
    {
        CUniqueFd fdDefault;
        TEST_CHECK(!static_cast<bool>(fdDefault));
        TEST_CHECK_EQ(-1, fdDefault.fi32_Get());
        fdDefault.fv_Reset(); // Resetting invalid fd must be a safe no-op
        TEST_CHECK_EQ(-1, fdDefault.fi32_Get());

        int ai32_Pipe[2]{-1, -1};
        TEST_CHECK_EQ(0, ::pipe(ai32_Pipe));
        const int i32_ReadFd = ai32_Pipe[0];
        const int i32_WriteFd = ai32_Pipe[1];

        {
            CUniqueFd fdRead(i32_ReadFd);
            CUniqueFd fdWrite(i32_WriteFd);
            TEST_CHECK(static_cast<bool>(fdRead));
            TEST_CHECK(static_cast<bool>(fdWrite));
            TEST_CHECK(sfb_IsFdOpen(i32_ReadFd));
            TEST_CHECK(sfb_IsFdOpen(i32_WriteFd));

            // Resetting to -1 immediately closes managed descriptor
            fdWrite.fv_Reset();
            TEST_CHECK(!static_cast<bool>(fdWrite));
            TEST_CHECK(!sfb_IsFdOpen(i32_WriteFd));
        }
        // Scope exit must close i32_ReadFd automatically
        TEST_CHECK(!sfb_IsFdOpen(i32_ReadFd));
    }

    // 2. CUniqueFd move construction, move assignment, and self-move
    {
        int ai32_PipeA[2]{-1, -1};
        int ai32_PipeB[2]{-1, -1};
        TEST_CHECK_EQ(0, ::pipe(ai32_PipeA));
        TEST_CHECK_EQ(0, ::pipe(ai32_PipeB));

        CUniqueFd fdA0(ai32_PipeA[0]);
        CUniqueFd fdA1(ai32_PipeA[1]);
        CUniqueFd fdB0(ai32_PipeB[0]);
        CUniqueFd fdB1(ai32_PipeB[1]);

        const int i32_RawA0 = fdA0.fi32_Get();
        const int i32_RawB0 = fdB0.fi32_Get();

        // Move constructor transfers ownership and invalidates source
        CUniqueFd fdMoved(std::move(fdA0));
        TEST_CHECK(!static_cast<bool>(fdA0));
        TEST_CHECK_EQ(-1, fdA0.fi32_Get());
        TEST_CHECK_EQ(i32_RawA0, fdMoved.fi32_Get());
        TEST_CHECK(sfb_IsFdOpen(i32_RawA0));

        // Move assignment closes previous target (i32_RawA0) and acquires i32_RawB0
        fdMoved = std::move(fdB0);
        TEST_CHECK(!static_cast<bool>(fdB0));
        TEST_CHECK(!sfb_IsFdOpen(i32_RawA0));
        TEST_CHECK_EQ(i32_RawB0, fdMoved.fi32_Get());
        TEST_CHECK(sfb_IsFdOpen(i32_RawB0));

        // Explicit self-move guard check
        CUniqueFd* p_Self = &fdMoved;
        fdMoved = std::move(*p_Self);
        TEST_CHECK_EQ(i32_RawB0, fdMoved.fi32_Get());
        TEST_CHECK(sfb_IsFdOpen(i32_RawB0));

        // Release remaining descriptors explicitly
        fdMoved.fv_Reset();
        fdA1.fv_Reset();
        fdB1.fv_Reset();
        TEST_CHECK(!sfb_IsFdOpen(i32_RawB0));
    }

    // 3. UniqueDir: null pointer safety and valid directory stream release
    {
        UniqueDir pNullDir(nullptr);
        TEST_CHECK(!pNullDir);
        pNullDir.reset();

        CScopedTempDir tempDir("raii_dir_");
        {
            UniqueDir pDir(::opendir(tempDir.fkr_Path().c_str()));
            TEST_CHECK(static_cast<bool>(pDir));
        }

        UniqueDir pMissingDir(::opendir((tempDir.fkr_Path() / "does_not_exist").c_str()));
        TEST_CHECK(!pMissingDir);
    }

    // 4. UniquePipe: null pointer safety and valid pipe release
    {
        UniquePipe pNullPipe(nullptr);
        TEST_CHECK(!pNullPipe);
        pNullPipe.reset();

        {
            UniquePipe pPipe(::popen("true", "r"));
            TEST_CHECK(static_cast<bool>(pPipe));
        }
    }

    std::cout << "[PASS] TestRaiiHandles\n";
}
